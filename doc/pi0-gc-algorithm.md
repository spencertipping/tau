# π₀ GC algorithm
Values are allocated into the newest heap. When it overflows, we trigger a GC for that heap alone, tenuring downwards. Each downwards heap overflow may indirectly cause further GCs within those generations, all the way down to the oldest.

We don't track tenure-count per object (UTF9 doesn't make space), but we can track it per generation. This means one of every _n_ collections will tenure everything downwards instead of compacting. Since newer generations tend not to be as resizable, we can also tenure downwards anytime the live set is larger than some fraction of the newgen size.

(**NOTE:** the newgen-resizable thing is interesting. We don't resize newgen as a function of its live set, but we do temporarily expand it if we need to allocate a large object. This way large objects still follow generational lifetimes, unlike for instance the JVM G1 GC, which allocates them specially.)

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
The major parts of multi-generational GC. Some data dependencies are implicit, particularly `marked` and splice plans.

```cpp
void gc_mark(n)
{
  for (let e : externals)
    if (e.generation == n)
      mark_external(n, e);
  for (let r : marked[n])
    if (r.generation == n && r.flagged())
      trace(n, r);
}

void trace(n, r)
{
  mark_internal(n, r);
  if (wasnt_internally_marked_before(r))
    for (let c : r)
      if (c.flagged())
        trace(n, c);
}

plan build_plan(n, marked)
{
  // NOTE: marked is partitioned per generation
  marked[n].sort();
  set [root, contained] = solve(marked[n]);
  set to_inline         = singly_referenced(marked[n]) - contained;
  uN  live_set_size     = root.total_size();
  uN  excess_live       = tenure_threshold - live_set_size;

  // NOTE: no need to visit contained objects;
  // just stick to the root set for efficiency
  for (let m : root)
    for (let r : refs_in(m))
      if (to_inline.contains(r))
        // NOTE: we also update parent sizes and flags as
        // necessary when we splice an inline, hence m
        // argument here
        plan_splice_inline(m, r);

  // TODO: plan_move() is not part of the splice planning mechanism
  while (excess_live > 0)
    // NOTE: this plans actions against heap gen n + 1
    // (next oldest)
    excess_live -= plan_move(root.pop_front(), n + 1).size;

  // plan the rest of the live set here, since new generations
  // are at this point static (even if we collect older ones)
  for (let r : root) plan_move(r, n);
}

void execute_plan(marked, planned)
{
  for (let m : planned)
    // copy object to destination, applying splices
    copy_and_adjust(m);

  for (ref &r : external) r = planned[r];
  for (let  x : marked)  // x is old object
  {
    let y = planned[x];  // y is new object
    for (ref &r : refs_in(y)) r = planned[r];
  }
}

void gc(n)
{
  // Because GC always begins with newgen, we just mark this
  // generation of references -- not everything newer
  for (; needs_gc(n); ++n)
  {
    gc_mark(n);
    planned += build_plan(n, marked);
  }
  execute_plan(marked, planned);
}
```

Let's get into some parts of this.


### Tenuring and inlining
Notice that we inline (within same-generation) before tenuring. This is important: the primary goal is to improve locality, secondary is to achieve optimum generation-packing.


### Mark structure
`marked[n]` is just a vector of references backed by an unordered membership set, in this case `umap<ref, uset<ref>>`. `uset<ref>` accumulates _internal_ references; that is, things we need to rewrite. External references aren't added here because they don't have heap-internal addresses. If an object has only external references, it will be present in the map but have an empty `uset<ref>`. `singly_referenced()` returns objects whose `uset<ref>` size is exactly 1.

If an object is contained within another _marked_ object, then the child is immobile and ineligible for inlining.

Note that from the GC's perspective, `root` and `contained` are only with respect to the marked set; so if you have `a = tuple(b = tuple(c = tuple(d)))` and hold references to only `b`, `c`, and `d`, then `b` is a root even though it wasn't in its initial generation.


### Plan structure
`planned` needs to provide three pieces of information:

1. Which bytes should be copied to which (relative) new locations
2. Which bytes need to be patched, and to which values
3. For a given old-space address, what is the corresponding new-space address

(1) provides inputs to `memcpy` and informs heap sizing, (2) allows us to fix up UTF9 container sizes and flags, and (3) makes it easy to rewrite references.

`planned` stores a separate splice map per generation. Each of those splice maps contains two sorted vectors:

+ `offsets`: a vector of `(old_address, length, new_address)` tuples
+ `patches`: a vector of `(new_address, byte)` pairs

`offsets` provides enough information to drive `memcpy` and to calculate new addresses.
