# π₀ heap compression
Because π₀ data structures are semantically immutable, we can, in principle, replace duplicated UTF9 values with references. If we wanted to do this, we could use an efficient deduplication strategy like `quickdupe` to rapidly reject duplication candidates during GC, then fuse whatever remains.

Heap compression allows us to deal with two otherwise inconvenient scenarios:

1. Values are sent down ζ, which results in them being fully expanded (right now, anyway)
2. A value is unquoted from `bytes`, which also results in full expansion
