# Undervisor Typecasting Verification Design
By [__Savvy Raghuvanshi__](https://savvy.bio), under the guidance of [__Prof. James Mickens__](https://mickens.seas.harvard.edu/).


## High level overview

When compiling code, we collect type information metadata and [keep track](#what-does-it-mean-to-actually-keep-track-of-a-code-operation) of code locations where objects are either created or casted between types. Then, at runtime, when one of these code locations is reached, the undervisor does the following:
- If the instruction corresponds to an allocation, it stores a mapping, where the key is the address of the allocated object and the value is the type of the allocated object
- If the instruction corresponds to a casting, the undervisor checks whether the source and destination types form a valid cast and terminates the top-half code if not.

The actual type-casting verification strategy is taken from the [HexType paper](https://acmccs.github.io/papers/p2373-jeonA.pdf), while our contribution is the adaption of HexType's strategies to the undervisor. In particular, we address the following challenges:
- Running type casting verification in a different address space from running client code
- Running type casting verification with access only to the stream of executing instructions and register state of the target program (as opposed to full access to the program's memory)


## Compile-time Operations

While compiling code, we need to know which pairs of source and destination types can be safely casted. To this end, during compile time, we enumerate all the types defined in the program (the compiler has to do this anyways). Furthermore, we analyze the memory layout of all classes defined in the program --- this is useful to enumerate phantom classes, as well as to determine which classes might have objects that share the same base pointer.

We also set up a table containing mappings from source types to valid destination types for casts. This table is then loaded into the undervisor when a program begins executing and is referenced during runtime to determine the validity of each cast.


### Which potential casts are safe?

We can always safely _upcast_, i.e. cast from a class to one of its superclasses. However, we cannot safely _downcast_, i.e. cast an object from a class to one of its derived classes, unless the target class has the same memory layout as the parent. Such child classes are known as _phantom classes_. We permit casting to phantom classes, just like the HexType authors, since they do not enable typecasting vulnerabilities and have practical applications.

Note that we can also always cast from a class to itself.

Less trivially, a cast from an object to its first member is possible if they share the same base pointer (this requires the encapsulating object not to have a vtable pointer stored at its base). As far as I can tell, such a cast is not valid C++ and relies on implementation-defined (though de-facto standard) behavior. In light of this, we do not allow such casts; if this restriction is later lifted, we can specifically allow such casts.

Therefore, to enumerate the valid target classes for casting from a given class `A`, we simply recursively enumerate all the valid target classes for all superclasses of `A`.

#### Technically illegal casts

Note that we allow some technically illegal, but practically harmless, casts. For instance, if a class `A` has a phantom child `P` and a derived child `D`, then we allow a cast from `D` to `P`, even though this is technically an invalid cast between sibling classes. However, since `A` and `P` share the same data layout, we claim (like the HexType authors) that a cast from `D` to `P` will not create any typecasting vulnerabilities.


### What does it mean to actually "keep track" of a code operation?

After initialization of a _dyad_ (i.e. combination of a top-half program and its bottom-half monitor), an undervisor's bottom-half program has access only to the registers of the top-half program, as well as a stream of instructions from the top-half as they execute. The undervisor's only ability to interface with top-half code is to prevent an instruction from retiring, consequently killing the program. Therefore, the monitor code is naturally event-driven, with events corresponding to instructions in the top-half and the output being a decision on whether to allow each instruction to retire.

We wish to enforce some security properties --- namely, that the typecasts in the top-half source code are safe --- under these constraints. To that end, it is important for the bottom-half code to know where in the instruction stream these casts occur, what source and destination types are involved, and which object in memory is being casted.

During the transformation from source code to machine instructions, we need to preserve the associations between this metadata and locations in the code. These associations are then loaded into the bottom half. During execution, as the instruction pointer (`%rip`) advances, the monitor checks if the instruction is "interesting", and if so it looks up the metadata and performs the desired verifications.

One of the challenges of this research project is actually passing the information to the bottom-half about which `%rip` values are "interesting". Doing so requires invasive modifications to the build system, which we describe here.

In summary, during compilation, we want to mark certain locations in the code as "interesting" and store some metadata about them that the undervisor will know at load-time.


#### Working with the AST

When the Clang compiler is invoked on a C++ program, it parses the source code into an [Abstract Syntax Tree](https://clang.llvm.org/docs/IntroductionToTheClangAST.html). The Clang AST is the right level of abstraction for us to mark interesting code locations --- if we operate on raw (or even pre-processed) source code, we might run into problems parsing complicated, nested expressions within a single line of code, like the following:
```cpp
if(static_cast<BadType*>(ptr*)->func()) { do_something(); }
```
At the same time, if we wait until further along the build pipeline to mark interesting code locations, we start clashing with optimizations --- most casts will be compiled into no-ops, and many function calls (like constructors) will be inlined. Thus, the code operations we are interested in marking may either disappear or be duplicated.

To mark an interesting code operation, we wrap its AST node inside a new type of "undervisor node". As an example, consider the following line of code (line number 22 in the original source):
```cpp
    SimpleDerived1* sd1 = static_cast<SimpleDerived1*>(sb1);
```
In the Clang-generated AST, this statement translates to:
```
|-DeclStmt <line:22:5, col:60>
| `-VarDecl <col:5, col:59> col:21 sd1 'SimpleDerived1 *' cinit
|   `-CXXStaticCastExpr <col:27, col:59> 'SimpleDerived1 *' static_cast<class SimpleDerived1 *> <BaseToDerived (SimpleBase)>
|     `-ImplicitCastExpr <col:56> 'SimpleBase *' <LValueToRValue> part_of_explicit_cast
|       `-DeclRefExpr <col:56> 'SimpleBase *' lvalue Var 0x563044ce06f8 'sb1' 'SimpleBase *'
```
We simply wrap the cast in a new undervisor node:
```
|-DeclStmt <line:22:5, col:60>
| `-VarDecl <col:5, col:59> col:21 sd1 'SimpleDerived1 *' cinit
|   `-UndervisorStaticCast <col:27, col:59> sb1 to 'SimpleDerived1 *'
|     `-CXXStaticCastExpr...
```
We also ensure that this node is _transparent_ to the compiler --- it does not affect optimizations or output any top-half instructions. Instead, it simply stores the metadata we want associated with the cast operation (in this case, the source variable and the destination type of the cast). Wherever code is duplicated or inlined, the node gets copied as well, and if its contents are optimized away it will contain an empty body but will not be deleted itself.


#### LLVM

However, most of Clang's optimizations do not occur at the AST level, but rather within LLVM. To that end, we [create a new type of LLVM Metadata](http://llvm.1065342.n5.nabble.com/creating-new-Metadata-td30085.html). This can then be [programmatically accessed](http://llvm.org/docs/SourceLevelDebugging.html#ccxx-frontend). Note that
> A transformation is required to drop any metadata attachment that it does not know or know it can’t preserve...
>
> Metadata attached to a module using named metadata may not be dropped, with the exception of debug metadata (named metadata with the name !llvm.dbg.*).

(Source: [LLVM Language Reference](https://llvm.org/docs/LangRef.html#metadata-nodes-and-metadata-strings))

Therefore we either need to make all of our undervisor metadata named or alter the LLVM transformations to simply ignore (but not delete) it.

Also, note that while we keep the undervisor metadata if an operation (such as a cast) compiles to a no-op, we want to delete the metadata if its entire basic block is compiled out! In that case, doing any sort of undervisor checks would be incorrect, since the code in question would not even be running.

__TODO__: It is still an open problem for me on how to deal with this optimization case --- I think it would entail modifying the optimization passes that are able to delete entire basic blocks, but I'm not 100% sure.


#### Assembler Output

Each instance of undervisor metadata in the LLVM intermediate representation will eventually be turned into an assembler directive in the code generation step's output. For instance, if the aforementioned undervisor node's body is optimized away, then the generated assembly will just be:
```asm
// static_cast (compiled to no-op)
.undervisor.typecasting_verification.static_cast from_addr=%rbx to_type='SimpleDerived1 *'
```
In this example, `%rbx` is the register that contains the address of the source object to be casted, and `SimpleDerived1 *` is the destination type. If we wish to keep track of another code operation (such as a constructor or destructor), we simply adapt the generated undervisor AST node and assembler directive to contain the relevant metadata.

The code generator's output (a `.S` file) will be passed to a bottom-half code generator, which will parse out the undervisor-specific directives and use them to construct the initial state of the monitor code (i.e. the set of valid typecasts, as well as interesting `%rip` values and their associated metadata). The top-half assembler will ignore these directives (more specifically, we can just strip them out with a simple shell script before invoking the assembler) and generate the top-half binary as usual.


##### Useful Links

[Overview of Clang Internals](https://cppdepend.com/blog/?p=321)


### What code operations are we interested in?

When compiling code, we [keep track](#what-does-it-mean-to-actually-keep-track-of-a-code-operation) of allocations and casting operations. Keeping track of allocations allows the undervisor to glean information about the type of each allocated object and track this information throughout program execution. Keeping track of casting operations is necessary for the undervisor to know when to actually perform runtime typecasting checks.


#### Allocations

During compilation, we keep track of the following types of (de)allocations:
- `new` allocations (constructors)
- placement new
- `delete` operations (destructors)
- `new[]` and `delete[]` operations
- move and copy constructors
- stack allocations
- global allocations
- `static_cast` or `reinterpret_cast` (from a previously untracked pointer)
- `malloc()` and `free()` (optional)


##### New, Delete, Constructors, and Destructors

When we use the `new` operator to make a [dynamic-duration](https://en.cppreference.com/w/cpp/language/storage_duration#Storage_duration) object (essentially an object on the heap), that translates to the compiler allocating some appropriately-sized memory using `operator new` (which is essentially similar to `malloc()` in that it allocates raw memory), and then calling the appropriate constructor. Placement new is similar, but instead of allocating memory with `operator new`, it merely uses the memory location passed to it and calls the constructor there.

Consider the following example program:
```cpp
#include <string>
#include <vector>

class SimpleBase {
public:
    int sb_member1;
    std::string sb_member2;
    SimpleBase() :sb_member1(rand()) {};
    ~SimpleBase(){};
};

class SimpleDerived1 : public SimpleBase {
public:
    char sd1_member1;
    std::vector<int> sd2_member2;
    SimpleDerived1(){};
    ~SimpleDerived1(){};
};

int main() {
    volatile SimpleBase* sb1 = new SimpleBase();
    return sb1->sb_member1;
}
```
When compiled with `-O2`, it compiles to the following assembly:
```asm
main: # @main
  push rbx
  // begin allocating memory
  mov edi, 40
  call operator new(unsigned long)
  // end allocating memory
  // execute constructor body
  mov rbx, rax
  call rand
  mov dword ptr [rbx], eax
  lea rax, [rbx + 24]
  mov qword ptr [rbx + 8], rax
  mov qword ptr [rbx + 16], 0
  mov byte ptr [rbx + 24], 0
  // end constructor body
  mov eax, dword ptr [rbx]
  pop rbx
  ret
```

Note that the constructor of `SimpleBase` is inlined, but the call to `operator new` is not. I believe that the call to `operator new` can never actually be inlined, since we need to link in the library containing it (`libstdc++`).

Instead of tracing calls to `operator new` in the undervisor, we only track objects that have been initialized --- to that end, we [keep track](#what-does-it-mean-to-actually-keep-track-of-a-code-operation) of the _terminations_ of all constructors. This allows us to avoid special-casing for different memory allocation systems, including bare `malloc()` or a memory-pool initialized with placement new.

When we reach the end of an executing constructor for an object, we create a mapping in the undervisor from the base address of that object to the type whose constructor just ran. If our bottom-half map already contains a different type associated with that base address, we raise an error condition and abort. Therefore, we maintain a canonical mapping from virtual top-half addresses to object types.

However, there is the complication that objects from two different classes might share the same base pointer --- for instance a `SimpleDerived1` object and its constituent `SimpleBase` object. This can happen during inheritance or if one object contains another as its first data member. In either case, we will see nested execution of constructors. According to C++ rules, the member fields are [initialized in order of their declaration](https://stackoverflow.com/a/7539330/2378475) and destructed in the reverse order. Therefore, we keep track of when each constructor both starts and finishes execution to be able to account for nested construction. If a constructor is called with the same base pointer as another constructor _while the first is already executing_, then the inner constructor is assumed to be either a base class or the first member field and is therefore ignored.

This approach ensures that our undervisor address-to-type map always contains the most specific, valid type that each object was allocated as. This property is critical to prevent both false negatives and false positives in our typecasting verification.

Note that while our approach ensures we can initialize all of an object's member fields in the constructor, we must watch out for the specific scenario where we need to re-initialize the first member field of an object after the constructor for the enclosing object has already completed, such as in the following (unidiomiatic) code example:
```cpp
#include <cstddef>
#include <cstdio>
#include <new>

struct ExternalMemory {
    ExternalMemory(size_t size) : memloc(new char[size]), size(size) {}
    ~ExternalMemory() { delete[] memloc; }
    size_t size;
    char* memloc;
};

class HardwareManager {
public:
    HardwareManager() : removable(1000), present(true) {};
    void remove() {
        if(present) removable.~ExternalMemory();
        present = false;
    }
    void add() {
        if(!present) removable = *(new(&removable) ExternalMemory(2000));
        present = true;
    }
    void print() {
        printf("Hardware Manager: ");
        if(present) {
            printf("\"removable\" present: size %lu\n", removable.size);
        } else {
            printf("\"removable\" not present.\n");
        }
    }
private:
    ExternalMemory removable;
    bool present;
};

int main() {
    auto hwm = new HardwareManager();
    hwm->print();
    hwm->remove();
    hwm->print();
    hwm->add();
    hwm->print();
}
```
The output is as follows:
```cpp
Hardware Manager: "removable" present: size 1000
Hardware Manager: "removable" not present.
Hardware Manager: "removable" present: size 2000
```
Note that in the above example, the first member variable of `hwm` has both its destructor and constructor called after `hwm` has already been initialized.

To prevent false positives in our typecasting verification in such scenarios, we ignore the invocation of any such constructor (i.e. that of the first member field or base object, if it shares the base pointer with the enclosing object) and do not change the underlying type mapped to that address.

We proceed similarly in the destructor case --- if a destructor is called on a specific base address, we check if it is compatible with (i.e. the same class, its base class, or the first member's class) its mapping in the undervisor. If the mapping is incompatible or doesn't exist, we abort. Otherwise, we continue normally. Note that since [destructors run in the opposite order of construction in inheritance](https://isocpp.org/wiki/faq/multiple-inheritance#mi-vi-dtor-order), we remove an address' mapping in the undervisor only when its most ancestral class' destructor completes.

__Note__: Our design properly handles deleted [constructors](https://stackoverflow.com/questions/13654927/why-explicitly-delete-the-constructor) and [destructors](https://stackoverflow.com/questions/18847739/how-does-delete-on-destructor-prevent-stack-allocation) --- an object cannot have all of its constructors deleted in C++ and still be instantiated, or the program will not compile. Similarly, if a class' destructor is deleted, objects of that class can never be destroyed, and we do not have to remove any such mappings (from addresses of the objects to their type) from the bottom half at runtime.

The metadata we need to keep track of for constructors and destructors are simply the type and address being operated on. For a more detailed discussion on how we actually keep track of the instruction pointer values where constructors and destructors begin and end, see [Keep Track](#what-does-it-mean-to-actually-keep-track-of-a-code-operation).

Note that the move and copy _assignment operators_ need not be tracked, since calling an assignment operator does not actually update the type of the underlying object.

Finally, we consider move and copy _constructors_ --- in both cases, we are essentially calling a constructor as in the standard case, but with some more specification regarding initial values of the created object. Copy constructors do not affect the source object ( __TODO__ : can this be overridden somehow? maybe with an evil `const_cast`), so we do not need to modify an undervisor mapping for the source object's address. Furthermore, move constructors operate on rvalues, so we do not need to worry about the source object ( __TODO__ : is this even true? It's possible that something like `std::move` might let us maul an lvalue).


__TODO:__ add more/better code samples in this section


###### Alternate proposal: Static Linking Only

Note that if we statically link with `libstdc++`, then we know the virtual address of that call to `operator new` straight from the executable (as well as all of its instructions) before the program is even loaded, and we can prepare the undervisor to watch for a `%rip` equalling that address during top-half execution.

Therefore, for simplicity, we will work only with statically linked top-half executables for the time being. Once we know that the undervisor works conceptually, we can investigate dynamic linking and compiling to create position-independent code to compute interesting top-half instruction pointer values at load time.

Some good resources that helped me understand static vs dynamic linking:
- [https://dustin.schultz.io/how-is-glibc-loaded-at-runtime.html](https://dustin.schultz.io/how-is-glibc-loaded-at-runtime.html)
- [https://stackoverflow.com/questions/3322911/what-do-linkers-do](https://stackoverflow.com/questions/3322911/what-do-linkers-do)
- [https://stackoverflow.com/questions/26103966/how-can-i-statically-link-standard-library-to-my-c-program](https://stackoverflow.com/questions/26103966/how-can-i-statically-link-standard-library-to-my-c-program)
- [https://stackoverflow.com/questions/5469274/what-does-plt-mean-here](https://stackoverflow.com/questions/5469274/what-does-plt-mean-here)



##### Raw allocations: `malloc()` and `free()`
Note that memory allocated with `malloc()` or `calloc()` has to be converted to an object type (if necessary) via one of the following operations:
- placement new
- `static_cast`
- `reinterpret_cast` (not recommended)

At the site of that cast (or the calling of the constructor via placement new), we keep track of what object type corresponds to that memory address in the usual way.

Notably, we can also easily avoid use-after-free bugs if we [keep track](#what-does-it-mean-to-actually-keep-track-of-a-code-operation) of the arguments to and return values from calls to `malloc()` and `free()`.


##### Stack and Global Allocations

These shouldn't be different from normal heap allocations, since we are working with raw virtual addresses.


##### "Allocations" via Casts

Note that certain "casts" in source code can actually serve to create objects! For instance, deserialized (or otherwise initialized) data can be validly casted into an object. Therefore, we need to consider both the case when a cast creates a new object, and when it merely rebrands an existing one.

In the undervisor, when we come across a `static_cast` or `reinterpret_cast`, we first check if the source virtual address being casted is already the base address for some object we are tracking. If so, we follow the casting procedures described in the section on [Casts](#casts).

On the other hand, if the source address is currently untracked (and we are casting to some object type), we assume that the cast is creating a new object from memory that was somehow previously allocated. In this case, we add a mapping to the undervisor from the given address to the destination type. If the destination type of the cast is a builtin like `char*` or `int*`, we ignore the cast and do not create an entry in the undervisor's map.


#### Casts

We keep track of the following types of casts:
- `static_cast`
- `reinterpret_cast`
- C-style cast
- (optional, potentially future work) `dynamic_cast`

When we reach a casting instruction, we look up the mapping associated with the source address of the cast. If we are casting to an invalid destination type for that object, the undervisor aborts the top-half code and cleans up. If the virtual address already corresponds to a tracked object and the destination type is _valid_ for that object, then program execution continues normally.

__TODO__: possibly expand on this more


## Limitations, Questions, and Future Work

NESTED OBJECTS??? If a class doesn't have a vtable (e.g. `SimpleDerived3` and `SimpleDerived2`), then the base pointer of its first member variable is the same as the base pointer for the whole object itself!!

[Dealing with position-independent code (needed for ASLR), as well as dynamic linking.](#alternate-proposal-static-linking-only)

NOT Multithreading safe

Will this approach work for folks who override `operator new` either globally or for a specific class?

TODO: Self-modifying code? JIT? --- does any other solution even keep track of this?

Will this work with placement delete (!) ?

Overloaded assignment operators (like between multiple classes)?

Avoiding double indirections!

Smart pointers???

- Write up (dis)advantages with respect to Caver/HexType

- Do C-style casts get translated into either `const_cast`, `static_cast`, or `reinterpret_cast`, in the Clang AST?

- We do not deal with inline asm LOL


## Other Notes

We can use the output assembler directives (i.e. disable the "Directives" filter in [godbolt](https://godbolt.org)) to pinpoint specific assembly instructions to their original locations in source files.

