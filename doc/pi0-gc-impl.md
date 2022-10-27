# π₀ GC implementation
Starting with invariants:

1. Heap objects are stored in [UTF9](utf9.md) format.
2. Every reference must point backwards/downwards -- that is, towards an earlier same-gen address or an older generation.
3. Heap objects cannot be modified once written, with the exception of primitive vector/tensor data.
4. Some references are inlined during GC compaction.


## Algorithm outline
For a single generation without tenuring:

1. Mark all externally-referenced objects and their references
2. Make a set of pointers (not objects) that will be inlined
3. Calculate post-inlining live set size, make new-space
4. Copy live objects to new space, rewriting reference addresses as we go
5. Swap new-space into the heap, dispose of old-space

(4) can happen as a single step because we calculated the new size of each object in step (3).


## Inlinable objects
Any object not contained by another _marked_ object can be inlined into its earliest reference site, provided that none of its contained referents would be moved forward beyond their references (due to invariant 2).

**Q:** is there a simple way to ensure we don't violate the forward-reference constraint? I think we need to inline any broken references to contained objects, but we could also take other strategies: don't inline anything with marked children, or don't inline beyond any child's reference boundary. Of these, the former is probably simpler to implement.


## Tenuring
**TODO:** design for tenuring -- it should be a simple change to step (4)
