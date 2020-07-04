# Undervisor Typecasting Verification Design

#### High level overview

When compiling code, we collect type information metadata and keep track of code locations where objects are either created or casted between types. Then, at runtime, when those code locations are reached, the undervisor checks whether the source and destination types form a valid cast.


## Details

When compiling code, we place markers after the following types of allocations and casting instructions to keep track of the `%rip` value where the operation is completed: 
- `new` allocations
- stack allocations
- global allocations
- `static_cast`
- `reinterpret_cast`
- placement new
- `malloc()` and `free()`
- (optional) `dynamic_cast`

In order to keep track of heap allocations, we implement a custom memory allocator.
```cpp
// test code block
```
