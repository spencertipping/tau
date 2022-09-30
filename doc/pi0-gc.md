# π₀ GC
π₀ programs are stack-oriented bytecode, so we need to track their values accordingly. We do this with the usual three-tier setup:

1. A stack for immediate expressions
2. A stack of frames for local variables
3. A heap for indefinite lifetimes

Unlike Perl and Python, pointers are rare and we're free to move heap-allocated values. π₀'s GC is a cheap copying mark/sweep collector.

All values are encoded as UTF9 when stored, which sounds like a problem because UTF9 doesn't support pointers-to-things; each value is fully serialized inline. Luckily the [UTF9 spec](utf9.md) reserves a block of values with bit-prefix `11100fss` -- `fss` encodes the flag and size -- that π₀ is free to use in any way whatsoever. See [π₀ UTF9](pi0-utf9.md) for details about how those values are allocated. And we use this block of UTF9s to define pre-GC references that don't involve full copies; see [lazy slicing](#lazy-slicing).


## UTF9 and copying GC
UTF9 is usually very cheap to GC because it needs to be neither scanned nor transformed when it is moved. This is the common "simple value" case. The exception is when the value contains a pointer to part of another value; then the former is "complex" and requires the usual trace-and-rewrite.

We don't support finalizers; all values are stateless.

Any value stored in a π₀-managed heap uses its flag field to indicate complexity. Or to say the same thing differently, all π₀ UTF9 internal values set the flag, and no other type of value is allowed to within the scope of a π₀ runtime.


## Immutability
All values are immutable, but references are in particular. That means the GC can track the total live-set obligation for each value individually at little additional cost, and we know the live-set size almost immediately by summing the stored size for each register or stack entry we're storing. This information is valuable for [π₁](pi1.md), which can use it to estimate the space complexity of algorithms. It's also useful internally, as it means we can have generational GC without write barriers -- there is nothing to write.

(Technically UTF9 `bytes`, `utf8`, vectorized primitives, and tensors can be modified in-place. In practice none of these matter because they can't store pointers to other objects. There are also no specific semantics about what happens if you refer to them with a slice operation; you can get a reference or a copy.)


## Lazy slicing
Back when I looked into these things recreationally, `substring()` tended to partition runtimes into two camps: those who execute it eagerly for _O(n)_ immediate time and space complexity, and those who execute it lazily for _O(1)_ time+space, but at the cost of pinning the operand even if the slice you've requested is quite small.

π₀ can have the best of both worlds, something I hope/assume is standard behavior on modern runtimes as well. All slices, which in this case generalizes to all sub-references, happen lazily for _O(1)_ time+space. Rather than pinning the operand, though, we just continue to refer to it for as long as it's in the live set. Once it would be collected, we copy out the relevant portion in place of the pointer. (π is also free to copy pointers out sooner, for instance if the rewriting overhead is high enough.)
