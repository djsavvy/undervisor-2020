# Undervisor Typecasting Verification Design
Savvy Raghuvanshi, under the guidance of James Mickens

#### High level overview

When compiling code, we collect type information metadata and keep track of code locations where objects are either created or casted between types. Then, at runtime, when one of these code locations is reached, the undervisor does the following:
- If the instruction corresponds to an allocation, it stores a mapping, where the key is the address of the allocated object and the value is the type of the allocated object
- If the instruction corresponds to a casting, the undervisor checks whether the source and destination types form a valid cast and terminates the top-half code if not.

The actual type-casting verification strategy is taken from the [HexType paper](https://acmccs.github.io/papers/p2373-jeonA.pdf), while our contribution is the adaption of HexType's strategies to the undervisor. In particular, we address the following challenges:
- Running type casting verification in a different address space from running client code
- Running type casting verification with access only to the stream of executing instructions and register state of the target program


## Details

### Compile-time Operations

While compiling code, we need to know which pairs of source and destination types can be safely casted. To this end, during compile time, we enumerate all the types defined in the program (the compiler has to do this anyways). We also set up a table containing mappings from source types to valid destination types for casts. This table is then loaded into the undervisor when a program begins executing and is referenced during runtime to determine the validity of each cast.

#### Which potential casts are safe?

We can always safely _upcast_, i.e. cast from a class to one of its superclasses. However, we cannot safely _downcast_, i.e. cast an object from a class to one of its derived classes, unless the target class has the same memory layout as the parent. Such child classes are known as _phantom classes_. We permit casting to phantom classes, just like the HexType authors, since they do not enable typecasting vulnerabilities and have practical applications.

Note that we can also always trivially cast from a class to itself.

Therefore, to enumerate the valid target classes for casting from a given class A, we simply recursively enumerate all the valid target classes for all superclasses of A.

__Note__: This enables us to make technically illegal casts. For instance, if a class A has a phantom child P and a derived child D, then we allow a cast from D to P, even though this is technically an invalid cast between sibling classes. However, since A and P share the same data layout, we claim that a cast from D to P will not create any typecasting vulnerabilities.


#### What code operations are we interested in?

When compiling code, we keep track of allocations and casting operations. Keeping track of allocations allows the undervisor to glean information about the type of each allocated object and track this information throughout program execution. Keeping track of casting operations is necessary for the undervisor to know when to actually perform runtime typecasting checks.

##### Allocations

During compilation, we keep track of the following types of allocations:
- `new` allocations
- placement new
- `delete` operations
- `new[]` and `delete[]` operations
- stack allocations
- global allocations
- `malloc()` and `free()`
- "move and copy operators" TODO: learn about these!
- `reinterpret_cast`


##### Casts

We keep track of the following types of casts:
- `static_cast`
- `reinterpret_cast`
- C-style cast
- (optional) `dynamic_cast`

(TODO) Note that `reinterpret_cast` shows up as both an allocation and a cast.


#### What does it mean to actually "keep track" of a code operation?

To keep track of an allocation or cast, we essentially store the address of the instruction pointer (`rip`) where that operation completes. For instance, (TODO: code example)

To accomplish this, we essentially place a marker after the (TOOD: finish this)


#### Source-level Modifications

(?)
In order to keep track of heap allocations in the bottom half, we implement a custom memory allocator. When compiling the custom memory allocator, we keep track of the instruction pointer where an allocated object's address is returned to the program --- TODO



### Runtime Operations

During runtime, the undervisor keeps a mapping between addresses and the types of objects allocated at those addresses.


## Limitations

TODO: Self-modifying code? JIT? --- does any other solution even keep track of this?
