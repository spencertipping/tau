# π GC
As outlined in [π interpreter](pi-interpreter.md), there are two ways for π programs to store [UTF9](utf9.md) values: the data stack and the global heap. The global heap is a straightforward random-access set of registers that refer to mutually disjoint values; it is mark/sweep collected as it overflows.

Stack memory is more interesting. It can be accessed randomly for reading, but can be modified only by using `push` and `pop`. This in principle gives us a way to rewind the allocation pointer whenever we `pop`, but in practice we can't because UTF9 values are pointers -- that is, they are always passed by reference. So they pin their memory until the result has been safely written somewhere.

A rewindable stack allocator provides access to _n_ of the most recently-pushed values and allocation space for a new one at the same time, while maintaining a bounded heap size.


## Dual stack strategy
```
a c e     a _         a cde     a _        ∅ abcde
b d _  →  b ∅ cde  →  b _    →  ∅ bcde  →  _

a c e     a c _      a ∅ cde     a _
b d _  →  b ∅ de  →  b _      →  ∅ bcde
```

The basic idea is that each value is pushed onto an alternating stack, allowing the other to be rewound _after_ the push is complete (at which point all data is safely written).

There will be unused memory, marked here with _∅_, but it is reclaimed incrementally as each stack is popped.

This works as long as nobody holds a reference to any stack item across the `push()`, which is illegal anyway as the underlying containers may need to be resized.
