# π GC
As outlined in [π interpreter](pi-interpreter.md), there are two ways for π programs to store [UTF9](utf9.md) values: the data stack and the global heap. The global heap is a straightforward random-access set of registers that refer to mutually disjoint values; it is mark/sweep collected as it overflows.

Stack memory is more interesting. It can be accessed randomly for reading, but can be modified only by using `push` and `pop`. This in principle gives us a way to rewind the allocation pointer whenever we `pop`, but in practice we can't because UTF9 values are pointers -- that is, they are always passed by reference. So they pin their memory until the result has been safely written somewhere.

A rewindable stack allocator provides access to _n_ of the most recently-pushed values and allocation space for a new one at the same time, while maintaining a bounded heap size.


## Allocation structures
+ General-purpose heap, with numbered global registers
+ Stack of local frames, each with numbered locals
+ Stack of immediate values
+ Stack of return addresses, which are unrelated to GC


## Inline stacks
We can make this work by having two separate linear allocators that alternate as we add values:

**TODO:** maybe explain this diagram better; it's a little obscure

```
a c e     a _         a cde     a _        ∅ abcde
b d _  →  b ∅ cde  →  b _    →  ∅ bcde  →  _

a c e     a c _      a ∅ cde     a _
b d _  →  b ∅ de  →  b _      →  ∅ bcde
```

The basic idea is that each value is pushed inline onto one allocator, allowing the other to be rewound _after_ the push is complete (at which point all data is safely written). This creates an implicit transaction boundary on every stack push: all popped values become invalid at that point, but no sooner.

There will be unused memory, marked here with _∅_, but it is reclaimed incrementally as each stack is popped.

**TODO:** more comprehensive benchmarking; so far the dual-stack allocator is a bit slower than a mark/sweep heap, so we may just go with (generational) heaps.


## UTF9 refs
It's very wasteful to copy values around all the time. To avoid this, we reserve a block of UTF9 values for the π interpreter that allows us to refer inside others. The GC needs to be aware that this is happening; otherwise we may not dig far enough into a value to dereference the pointer.

Heaps mark their values as "complex" or "simple" -- values arriving from elsewhere are simple because they have no inter-value references, whereas anything we construct that involves such a reference is marked as complex. This distinction impacts GC performance: only complex-marked values need to be deep-copied; everything else can be block-copied.

UTF9 refs don't provide a way around the fundamental limitation that pointers aren't real in π: if you do something like `x = (x, x, x, x)`, you'll use 4x the memory of the initial `x`; GC flattens everything and the heap is never stably compressed, e.g. by pointer aliasing.
