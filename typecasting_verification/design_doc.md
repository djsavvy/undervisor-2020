# Undervisor Typecasting Verification Design
Savvy Raghuvanshi, under the guidance of James Mickens

#### High level overview

When compiling code, we collect type information metadata and keep track of code locations where objects are either created or casted between types. Then, at runtime, when those code locations are reached, the undervisor checks whether the source and destination types form a valid cast.

The actual type-casting verification strategy is taken from the [HexType paper](https://acmccs.github.io/papers/p2373-jeonA.pdf), while our contribution is the adaption of HexType's strategies to the undervisor. In particular, we address the following challenges:
- Running type casting verification in a different address from running client code
- Running type casting verification with access only to the stream of executing instructions and register state of the target program


## Details

### Source-level Modifications

(?)
In order to keep track of heap allocations in the bottom half, we implement a custom memory allocator. When compiling the custom memory allocator, we keep track of the instruction pointer where an allocated object's address is returned to the program ---


### Compile-time Operations

#### Which potential casts are safe?

While compiling code, we need to know which pairs of source and destination types can be safely casted. To this end, during compile time, we enumerate all the types defined in the program (the compiler does this anyways). We also set up a table containing mappings from source types to valid destination types for casts. This table is then loaded into the undervisor when a program begins executing and is referenced during runtime.

When compiling code, we place markers after the following types of allocations and casting instructions to keep track of the `%rip` value where the operation is completed:
- `new` allocations
- stack allocations
- global allocations
- placement new
- `malloc()` and `free()`
- `static_cast`
- `reinterpret_cast`
- C-style cast
- (optional) `dynamic_cast`

During compilation, we generate a table containing all type relationships. During runtime, the undervisor keeps a mapping between addresses and the types of objects allocated at those addresses.

```cpp
// test code block
```
