# π₀ GC implementation
Starting with invariants:

1. Heap objects are stored in [UTF9](utf9.md) format.
2. Every reference must point backwards/downwards -- that is, towards an earlier same-gen address or an older generation.
3. Heap objects cannot be modified once written, with the exception of primitive vector/tensor data.
