# π GC
π programs are stack-oriented bytecode, so we need to track their values accordingly. We do this with the usual three-tier setup:

1. A stack for immediate expressions
2. A stack of frames for local variables
3. A heap for indefinite lifetimes

Unlike Perl and Python, pointers are rare and we're free to move heap-allocated values. π's GC is a cheap copying mark/sweep collector.

All values are encoded as UTF9 when stored, which sounds like a problem because UTF9 doesn't support pointers-to-things; each value is fully serialized inline. Luckily the [UTF9 spec](utf9.md) reserves a block of values with bit-prefix `11100sss` (`sss` encodes the size) that the π interpreter is free to use in any way whatsoever. And we do exactly that to add pre-GC references that don't involve full copies. See [lazy slicing](#lazy-slicing) for details.


## UTF9 and copying GC
UTF9 is usually very cheap to GC because it needs to be neither scanned nor transformed when it is moved. This is the common "simple value" case. The exception is when the value contains a pointer to part of another value; then the former is "complex" and requires the usual trace-and-rewrite.

Simplicity is an internal attribute that is automatically tracked for every managed value. π's GC can use some discretion to trade time for space by pre-simplifying values.


## Immutability
All values are immutable, but references are in particular. That means the GC can track the total live-set obligation for each value individually at little additional cost, and we know the live-set size almost immediately by summing the stored size for each register or stack entry we're storing. This information is valuable for [∏](Pi.md), which can use it to estimate the space complexity of algorithms. It's also useful internally, as it means we can have generational GC without write barriers -- there is nothing to write.

(Technically UTF9 `bytes`, `utf8`, vectorized primitives, and tensors can be modified in-place. In practice none of these matter because they can't store pointers to other objects.)


## Lazy slicing
Back when I looked into these things recreationally, `substring()` tended to partition runtimes into two camps: those who execute it eagerly for _O(n)_ immediate time and space complexity, and those who execute it lazily for _O(1)_ time+space, but at the cost of pinning the operand even if the slice you've requested is quite small.

π can have the best of both worlds, something I assume is standard behavior on modern runtimes as well. All slices, which in this case generalizes to all sub-references, happen lazily for _O(1)_ time+space. Rather than pinning the operand, though, we just continue to refer to it for as long as it's in the live set. Once it would be collected, we copy out the relevant portion in place of the pointer. (π is also free to copy pointers out sooner, for instance if the rewriting overhead is high enough.)
