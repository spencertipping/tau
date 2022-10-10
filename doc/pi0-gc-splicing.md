# π₀ GC splicing
π₀ GC uses a custom `o9` to copy objects from the old to the new heap. Simple objects are just `o9i9`, complex objects are spliced. Splicing is a little involved due to two intersecting constraints:

1. Some, but not necessarily all, references will be inlined, changing their size
2. Flagged parent sizes may change, introducing new splice points

The splice-rewrite operation makes a list of all splice points, copies contiguous regions, and then rewrites any object sizes as necessary. Because we track flagged objects up front, we also clear any flag bits when an object becomes fully simplified.


## Building splices
Nested references require that π₀'s GC track references sorted by location; that way we can quickly partition refs into "inner" vs "outer". The splice-set then provides a new offset map within each outer region.
