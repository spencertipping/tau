# UTF9 Φ scope
Objects that are valid only within the process that created them. The first byte indicates the object subtype, following bytes carry the data.

+ `00`: reserved
+ `01`: heap reference, not pinned
+ `02`: heap reference, pinned (don't automatically free)
+ `03`: file descriptor


## Non-portable heap refs
Some UTF9 values will be too large for the ζ being used to carry them. Rather than expanding ζ or failing, we can "box" the value into a specially-allocated `u8[]`, then store a reference to that `u8[]` into ζ. This is done with `std::malloc` and `std::free`, and these values are assumed to not be aliased into other structures such as π₀ heaps.

Heap references can be pinned, which modifies their typecode in place. This is an implementation detail that prevents the heap object from being freed automatically when the ζ containing the heap ref is advanced.
