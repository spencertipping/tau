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


### Downward tenuring
Values placed early in the generation should be tenured first on the grounds that they are older -- a poor man's cycle count. We may then decide to leave a remnant of the live set in the newer generation to avoid over-tenuring. This splitting must obey the old→new restriction, which happens naturally if we follow the age gradient.


### Oldest generation
Resized per compaction to some function of the live-set size, probably 2x. Oldgen is large and rarely resized, so we probably want to balance return-memory-to-system with spend-time-tracing. This is especially relevant when memory usage fluctuates.


### Inlining preference
A large, multiply-referenced value has multiple potential inlining sites. We may prefer one of these over the others due to various factors, but first let's outline some invariants:

1. If a value is referred to from multiple generations, it must be inlined into the oldest (via old → new pointer prohibition)
2. Because only one generation is collected at a time, a value can be inlined only into its original generation -- in this case, the one being GC'd; this is a special case of (1)
3. If the value has siblings that refer to other generations, its inlining preference is zero; nothing is gained (we may later duplicate the other children, but we'd use more space to do so)
4. A container with same-generation-reference children prefers to inline all of them proportional to its total number of children or, perhaps, cache lines

As an initial strategy, **every value with multiple sites should be inlined into the earliest possible heap position.** This reduces the amount of duplication required when we tenure it. Because every reference will exist by the time a pointer to it does, this just means that we never un-inline an already-inlined object (except to make it toplevel).


### Inlining size
For now we can treat this as an invariant: anything smaller than a cache line can be inlined. Although we could change it at runtime, we create future commitments when we do: we can inline a value (possibly duplicating it), but we can't un-duplicate it later without a lot of work. This means it isn't a free tunable.


## Rewriting
The final stage: all internal and external references are rewritten. Because values may be tenured into different generations, the rewrite-lookup table must span up to every address in every generation. This matters because it forces all compactions to happen before anything is rewritten; this way we're guaranteed to have the new address for every value before updating any pointers.


## Full outline
```cpp
void gc(n, execute_plan = true)
{
  // Step 1: mark external and internal references
  // Because GC always begins with newgen, we just mark this
  // generation of references -- not everything newer
  for (let e : externals) if (e.generation == n) mark(e);
  for (let r : marked)
    if (r.generation == n && r.flagged())
      trace(r);

  // Step 2: sort refs and solve for containment
  marked.sort();
  set [root, contained] = solve(marked);
  set to_inline         = singly_referenced(marked);
  uN  live_set_size     = root.sum();
  uN  excess_live       = tenure_threshold - live_set_size;

  // Step 3: construct the splice plan
  for (let x : root)
    for (let r : refs_in(m))
      if (to_inline.contains(r))
        // NOTE: we also update parent sizes and flags as
        // necessary when we splice an inline, hence m
        // argument here
        plan_splice_inline(m, r);

  // Step 4: construct the move plan
  while (excess_live > 0)
    excess_live -= plan_move(root.pop_front(), n - 1).size;

  // plan the rest of the live set here, since new generations
  // are at this point static (even if we collect older ones)
  for (let r : root) plan_move(r, n);

  // if planned moves cause parent to overflow, include parent
  // gc in our plan (but don't execute it)
  if (needs_gc(n - 1)) gc(n - 1, false);

  // Step 5: execute move/rewrite plan (only during the gc()
  // call for newgen)
  if (execute_plan)
  {
    for (let m : planned) execute(m);

    for (ref &r : external) r = planned[r];
    for (let  x : marked)  // x is old object
    {
      let y = planned[x];  // y is new object
      for (ref &r : refs_in(y)) r = planned[r];
    }
  }
}
```
