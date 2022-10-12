# π₀ GC algorithm
Values are allocated into the newest heap. When it overflows, we trigger a GC for that heap alone, tenuring downwards. Each downwards heap overflow may indirectly cause further GCs within those generations, all the way down to the oldest.

We don't track tenure-count per object (UTF9 doesn't make space), but we can track it per generation. This means one of every _n_ collections will tenure everything downwards instead of compacting. Since newer generations tend not to be resizable, we can also tenure downwards anytime the live set is larger than some fraction of the newgen size.

For implementation reasons, the number of generations is always a power of two, by default 4 -- so we reserve the top two bits to describe which generation contains each reference.


## Marking
`gc()` on the newest generation is easy: we go through each heap-view and mark any reference to the newest generation. Once we have the full external-reference set, we trace any flagged UTF9 values for more to-newgen references and mark those transitively -- again, ignoring anything the points to an older generation.

Marking an older generation is a little more involved because new objects can refer to older ones. We know that any newgen is already compacted; tenuring is the only situation where we `gc` oldgen, and we tenure only when (1) the newgen live set is larger than desired, or (2) we're feeling saucy and tenure everything in newgen just because. This matters because although we need to know which oldgen objects are referenced, we don't have to re-compact any newer generation; new→old references can be edited in place.

So, broadly, in order to `gc` generation _k_, we need the transitive mark set for _1..k_ inclusive, where _1_ is the newest generation. We can preemptively mark the first _n_ generations on every GC, or we can wait for an older-gen overflow and re-mark as necessary. I suspect the latter will be faster.


## Compaction
Throughout the marking process, we split references into "internal" and "external"; that is, references from the heap being compacted and references from outside that heap (e.g. stack, frames, other generations). We do this because compaction is allowed to inline some objects into others.


### Non-tenuring newgen
This is the simplest case. We copy most live objects as they are, inlining any references that have at least one heap-internal reference. The goals are to minimize heap size and to reduce the number of flagged UTF9 objects, saving effort on the next collection.

"Minimize flags" needs some context. Due to [splicing](pi0-gc-splicing.md), flagged objects impact mark effort but not compaction time. In particular, any value with a flag creates _O(n₁ + n₂ + ...)_ effort, where _n₁_ is the number of elements in the value's parent, _n₂_ in the element's grandparent, etc. Most of this is low-impact work -- just checking flag bits and skipping values -- but it's all opportunity for cache misses.


### Inlining preference
A large, multiply-referenced value has multiple potential inlining sites. We may prefer one of these over the others due to various factors, but first let's outline some invariants:

1. If a value is referred to from multiple generations, it must be inlined into the oldest (via old → new pointer prohibition)
2. Because only one generation is collected at a time, a value can be inlined only into its original generation -- in this case, the one being GC'd; this is a special case of (1)
3. If the value has siblings that refer to other generations, its inlining preference is zero; nothing is gained (we may later duplicate the other children, but we'd use more space to do so)
4. A container with same-generation-reference children prefers to inline all of them proportional to its total number of children or, perhaps, cache lines
