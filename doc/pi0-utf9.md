# π₀ UTF9
[UTF9](utf9.md) reserves the `11100___` prefix for π use. These values are non-portable, meaning that they will never leave a π₀ heap context; that means we can use native endianness and sizing. The only constraint is that we be able to remove any such values from UTF9s we're working with, e.g. before sending them across a [φ](phi.md).


## Value format
**NOTE:** this is not long-term stable; it may change per build, and may not be the same across running instances within the same Γ network.

In our case it's simple: all of these values are just pointers to other values within the same heap. That means we encode a single native-width, native-endian integer:

```
cb sb [uN value offset]
```

These are flagged as "complex", meaning that anything containing them will be flagged as well. This causes the GC to rewrite any container as it's copied into the new heap, which of course is somewhat expensive. The GC is allowed to inline any pointer at any time to prevent this overhead.
