# Undervisor Typecasting Verification Design
By [__Savvy Raghuvanshi__](https://savvy.bio), under the guidance of [__Prof. James Mickens__](https://mickens.seas.harvard.edu/).


## High level overview

When compiling code, we collect type information metadata and [keep track](#what-does-it-mean-to-actually-keep-track-of-a-code-operation) of code locations where objects are either created or casted between types. Then, at runtime, when one of these code locations is reached, the undervisor does the following:
- If the instruction corresponds to an allocation, it stores a mapping, where the key is the address of the allocated object and the value is the type of the allocated object
- If the instruction corresponds to a casting, the undervisor checks whether the source and destination types form a valid cast and terminates the top-half code if not.

The actual type-casting verification strategy is taken from the [HexType paper](https://acmccs.github.io/papers/p2373-jeonA.pdf), while our contribution is the adaption of HexType's strategies to the undervisor. In particular, we address the following challenges:
- Running type casting verification in a different address space from running client code
- Running type casting verification with access only to the stream of executing instructions and register state of the target program


## Compile-time Operations

While compiling code, we need to know which pairs of source and destination types can be safely casted. To this end, during compile time, we enumerate all the types defined in the program (the compiler has to do this anyways). Furthermore, we analyze the memory layout of all classes defined in the program --- this is useful to enumerate phantom classes, as well as to determine which classes might have objects that share the same base pointer.

We also set up a table containing mappings from source types to valid destination types for casts. This table is then loaded into the undervisor when a program begins executing and is referenced during runtime to determine the validity of each cast.


### Which potential casts are safe?

We can always safely _upcast_, i.e. cast from a class to one of its superclasses. However, we cannot safely _downcast_, i.e. cast an object from a class to one of its derived classes, unless the target class has the same memory layout as the parent. Such child classes are known as _phantom classes_. We permit casting to phantom classes, just like the HexType authors, since they do not enable typecasting vulnerabilities and have practical applications.

Note that we can also always cast from a class to itself.

Less trivially, a cast from an object to its first member is possible if they share the same base pointer (this requires the encapsulating object not to have a vtable pointer stored at its base). As far as I can tell, such a cast is not valid C++ and relies on implementation-defined (though de-facto standard) behavior. In light of this, we do not allow such casts; if this restriction is later lifted, we can specifically allow such casts.

Therefore, to enumerate the valid target classes for casting from a given class A, we simply recursively enumerate all the valid target classes for all superclasses of A.

#### Technically illegal casts

Note that we allow some technically illegal, but practically harmless, casts. For instance, if a class A has a phantom child P and a derived child D, then we allow a cast from D to P, even though this is technically an invalid cast between sibling classes. However, since A and P share the same data layout, we claim (like the HexType authors) that a cast from D to P will not create any typecasting vulnerabilities.


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

When we use the `new` operator to make a [dynamic-duration](https://en.cppreference.com/w/cpp/language/storage_duration#Storage_duration) object (essentially an object on the heap), that translates to the compiler allocating some appropriately-sized memory using `operator new` (which is essentially similar to `malloc()` in that it allocates raw memory), and then calling the appropriate constructor.

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

__TODO:__ add more/better code samples in this section


Note that the constructor of `SimpleBase` is inlined, but the call to `operator new` is not. I believe that the call to `operator new` can never actually be inlined, since we need to link in the library containing it (`libstdc++`).

Instead of tracing calls to `operator new` in the undervisor, we only track objects that have been initialized --- to that end, we track the terminations of all constructors. This allows us to avoid special-casing for different memory allocation systems, including bare `malloc()` or a memory-pool initialized with placement new.

When we reach the end of an executing constructor for an object, we create a mapping in the undervisor from the base address of that object to the type whose constructor just ran. If our bottom-half map already contains a different type associated with that base address, we raise an error condition and abort. Therefore, we maintain a canonical mapping from virtual top-half addresses to object types.

However, there is the complication that objects from two different classes might share the same base pointer --- for instance a `SimpleDerived1` object and its constituent `SimpleBase` object. This can happen during inheritance or if one object contains another as its first data member. In either case, we will see nested execution of constructors. According to C++ rules, the member fields are [initialized in order of their declaration](https://stackoverflow.com/a/7539330/2378475) and destructed in the reverse order. Therefore, we keep track of when each constructor both starts and finishes execution to be able to account for nested construction. If a constructor is called with the same base pointer as another constructor _while the first is already executing_, then the inner constructor is assumed to be either a base class or the first member field and is therefore ignored.

This approach ensures that our undervisor address-to-type map always contains the most specific, valid type that each object was allocated as. This property is critical to prevent both false negatives and false positives in our typecasting verification.

Note that while our approach ensures we can initialize all of an object's member fields in the constructor, we must watch out for the specific scenario where we need to re-initialize the first member field of an object after the constructor for the enclosing object has already completed ( __TODO__ : really need a code example here). To prevent false positives in our typecasting verification, we ignore the invocation of any such constructor (i.e. that of the first member field or base object, if it shares the base pointer with the enclosing object) and do not change the underlying type mapped to that address.

We proceed similarly in the destructor case --- if a destructor is called on a specific base address, we check if it is compatible with (i.e. the same class, its base class, or the first member's class) its mapping in the undervisor. If the mapping is incompatible or doesn't exist, we abort. Otherwise, we continue normally. Note that since [destructors run in the opposite order of construction in inheritance](https://isocpp.org/wiki/faq/multiple-inheritance#mi-vi-dtor-order), we remove an address' mapping in the undervisor only when its most ancestral class' destructor completes.

For a more detailed discussion on how we actually keep track of the instruction pointer values where {con,de}structors begin and end, see [Keep Track](#what-does-it-mean-to-actually-keep-track-of-a-code-operation).

Note that the move and copy _assignment operators_ need not be tracked, since calling an assignment operator does not actually update the type of the underlying object.

Finally, we consider move and copy _constructors_ --- in both cases, we are essentially calling a constructor as in the standard case, but with some more specification regarding initial values of the created object. Copy constructors do not affect the source object ( __TODO__ : can this be overridden somehow? maybe with an evil `const_cast`), so we do not need to modify an undervisor mapping for the source object's address. Furthermore, move constructors operate on rvalues, so we do not need to worry about the source object ( __TODO__ : is this even true? It's possible that something like `std::move` might let us maul an lvalue).


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


### What does it mean to actually "keep track" of a code operation?

It feels like the right level of abstraction to work at is the Clang AST, since we can avoid problems like parsing very complicated statements in source code (i.e. a cast inside an `if()` condition, like in the following code block).
```cpp
if(static_cast<BadType*>(ptr*)->func()) { do_something(); }
```
At the same time, if we process further along the build process than Clang's AST, we start clashing with optimizations --- most casts will be compiled into no-ops, and many function calls (like constructors) will be inlined.

To keep track of an allocation or cast, we essentially store the address of the instruction pointer (`rip`) where that operation completes. To accomplish this, we manipulate Clang's Abstract Syntax Tree. We place every constructor, destructor, and casting operation inside a new type of "undervisor node" in the AST. In a sense, this node is transparent to the compiler for the top half --- it does not affect optimizations or output any top-half instructions. Instead, this node just stores the metadata we want associated with that operation (e.g. the register where a cast's source address is stored, and the type we wish to cast it to). Wherever code is duplicated or inlined, the node gets copied as well (and modified as necessary), and if its contents are optimized away it will contain an empty body but will not be deleted itself. Each instance of such a node will eventually be turned into a label/directive in the assembler's output. The top-half linker will then ignore these labels/directives, but the bottom-half compiler will convert them into an event-driven state machine suitable for being run on the undervisor.

__TODO__: To keep track of arguments to a function, (no longer necessary?) ....

__TODO__: To keep track of source/destination types of a cast, ....

__TODO__: To keep track of constructors/destructors (address, type, etc.), ....

For instance, (TODO: code examples)



## Limitations, Questions, and Future Work

NESTED OBJECTS??? If a class doesn't have a vtable (e.g. `SimpleDerived3` and `SimpleDerived2`), then the base pointer of its first member variable is the same as the base pointer for the whole object itself!!

[Dealing with position-independent code (needed for ASLR), as well as dynamic linking.](#alternate-proposal-static-linking-only)

Will this approach work for folks who override `operator new` either globally or for a specific class?

TODO: Self-modifying code? JIT? --- does any other solution even keep track of this?

Will this work with placement delete (!) ?

Overloaded assignment operators (like between multiple classes)?

Avoiding double indirections!

Write up (dis)advantages with respect to Caver/HexType

Smart pointers???

Do C-style casts get translated into either `const_cast`, `static_cast`, or `reinterpret_cast`, in the Clang AST?

## Other Notes

We can use the output assembler directives (i.e. disable the "Directives" filter in [godbolt](https://godbolt.org)) to pinpoint specific assembly instructions to their original locations in source files.

