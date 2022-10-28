# π₀ GC implementation
Starting with invariants:

1. Heap objects are stored in [UTF9](utf9.md) format.
2. Every reference must point backwards/downwards -- that is, towards an earlier same-gen address or an older generation.
3. Heap objects cannot be modified once written, with the exception of primitive vector/tensor data.
4. Some references are inlined during GC compaction.


## GC in pictures
Starting with the single-gen "when/where can we inline" scenario:

```
                #       ← root set
     +-----+    |
     V     |    V
  a=(b) c=(d e) f=(g h)
  ^     ^          | |
  |     +----------+ |
  +------------------+
```

We can't inline `h` unless we also inline `d`. This means `h`'s inlining overhead is _|b|_, since it will occur in two places:

```
          #
     b₁   |       b₂  ← now we have two b's
     |    V       |
  c=(b e) f=(g a=(b))
  ^          |
  +----------+
```

This should happen only if _|b| ≤ ρ_, where _ρ_ is the inlining size threshold -- but presumably in that case we never would have had _d → b_ to begin with, as `b` would have been pre-emptively inlined. So we can ignore this case: `h` cannot be inlined in the above example.

What about un-inlining `b` from `a`? Then we'd have one of three cases, depending on how clever we're willing to be:

```
case 1:
            #
  +----+    |
  V    |    V
  b c=(d e) f=(g a=(i))
  ^ ^          |    |
  | +----------+    |
  +-----------------+

case 2:
          #
          |
     +----(-------+
     V    V       |
  c=(b e) f=(g a=(i))
  ^          |
  +----------+

case 3:
  #
  |
  V
  f=(c=(b e) a=(i))
        ^       |
        +-------+
```

Case (1) doesn't help us; net heap complexity remains the same and we have more deeply-nested flags. If anything it's a step backwards.

Case (2) is better but also complicated; we wind up splicing `a`, which was a simple object that could be moved with a single `memcpy`. Although the resulting heap doesn't lose any of that simplicity in the big picture -- presumably `c` is also simple now -- it requires us to disassemble and reassemble non-flagged roots from the original heap space.

I think it's worth saying we want case (3) if we can get it. Let's run with that and explore multigenerational GC.


### Multigenerational GC
Newgen is always on top.

```
                #         ← newgen root set
   +-------+    |
   V       |    V
a=(b c) d=(e f) g=(h i)
^    |  ^    |     | |
|    |  +----(-----+ |
+----(-------(-------+
     |       |              newgen
- -  +--+    |  - - - - - - ------------
        |    |              oldgen
     +--(----+
     |  |
     V  V
j=(k l) m=(n) ...
^          |
+----------+
```

As expected, we can tenure `a=(b c)` with no issues. Doing so allows us to then also tenure `d` and then `g` in that dependency order. So tenuring as such isn't complicated; the complexity once again comes from choosing which references to inline.

We can inline _ρ_-small references across generations -- untenuring by duplication -- but that would have happened already, so let's assume all references are large. Let's also assume we're willing to tenure both `a` and `d`. We initially have this:

```
#
|
V
g=(h i)                     newgen
   | |          - - - - - - ------------
   | |                      oldgen
   | |
   +-(--------------------+
     +------------+       |
                  |  +----(--+
                  V  V    V  |
j=(k l) m=(n) ... a=(b c) d=(e f)
^    ^  ^  |           |       |
+----(--(--+           |       |
     |  +--------------+       |
     +-------------------------+
```

Then old-gen compaction is free to run on its own, with newgen references updated as though they were a heap view. In other words, old-gen GC considers only the root set from newgen.

This is useful because it means that tenuring and old-gen GC/inlining are two separate processes, and inlining is the same regardless of which generation we're working with.


### Case-3 relocation
The optimal-rewriting strategy. Note that the ordering of `g` and `h` doesn't matter:

```
     original order       |       g/h flipped
                          |
                #         |                 #
     +-----+    |         |      +-----+    |
     V     |    V         |      V     |    V
  a=(b) c=(d e) f=(g h)   |   a=(b) c=(d e) f=(g h)
  ^     ^          | |    |   ^     ^          | |
  |     +----------+ |    |   |     +----------(-+
  +------------------+    |   |                |
                          |   +----------------+
                          |
  #                       |   #
  |     +-------+         |   |     +-----+
  V     V       |         |   V     V     |
  f=(c=(b e) a=(i))       |   f=(a=(b) c=(d e))
```

If we define an _optimal heap_ as one in which only _ρ_-large, multiply-referenced quantities have references, then case-3 implies that **every heap has an optimal representation.**

The algorithm to optimize a heap is actually pretty simple. We start with the root set and inline everything, creating a reference only when we've already written an object:

```
      +-------+
      V       |
f=(c=(b e) a=(X))

              ↑
              already inlined b, so make a reference here
```

This is great because (1) it's very simple, and (2) we don't have to post-trace the heap to rewrite reference addresses; every reference we create is to an object whose location has already been defined. Coding-wise it's as simple as the usual mark-in-place strategy.


## Native `i9`s of references
Code like this needs to be handled carefully:

```cpp
π0hnf f{z.h, 2};
i9    x = z.pop;     f << &x;
i9    i = x.first(); f << &i;
i9    e;  f << [&]() { e = x.next(); };
while (i < e)
{
  // ...
  i = i.next();
}
```

Drawing another picture, let's suppose our state looks like this:

```
                x      i
                |      |
                V      V
a=(b c) d=(e f) g=(h i j k l)
        ^              |
        +--------------+
```

Although logical operations on `i` should dereference through to `d`, we need to make sure any GC preserves _i ∈ g_ -- that is, we draw a distinction between "pointer to `j`" and "pointer to `d`" even though they're the same logical object. The constraint here is that unless _i_ is an uncontained root, _i.next()_ is semantically invariant across GC. Equivalently, GC preserves container/child relationships within the live set. (All of this is easy to do; we just keep track of the original addresses while we update the root set. But it's worth mentioning because it's important for FFI.)


## Algorithm outline
The pointer-containment constraint means that we need to understand all containment relationships before writing anything to new-space: in the diagram above, we could mark `j` as a root (which it is), but we need to wait until we've also marked `g` before writing either to the new heap. Because we have no ordering guarantees for the root set, this means we must fully mark the old heap before calculating the "true roots" for the new space.

High-level process:

1. Mark all roots
2. Make a set of "false roots" (contained by others)
3. Calculate full size of true roots, which will solve for new inlines
4. Allocate new-space
5. Copy true roots into new-space, which will build the new-location map
6. Update true and false roots


### False-root relocation
```
                  r₁     r₂
     +-------+    |      |
     V       |    V      V
a=(b c d) e=(f g) h=(i j k l)
          ^            |
          +------------+
```

_r₂ ∈ r₁_ is a false root whose location is determined as `h` copies its contents into new-space: in particular, each `o9`, one of which is `k`, will add its new address to the relocation map.

The situation is different if _r₂_ is a true root:

```
       r₃         r₁     r₂
       |          |      |
     +-(-----+    |      |
     V V     |    V      V
a=(b c d) e=(f g) h=(i j k l)
          ^            |
          +------------+
```

Here, _r₃_ is a true root, so we aren't obligated to preserve the relationship between `c` and `d`; so it's valid to GC into this state:

```
r₃ r₁            r₂
|  |             |
V  V             V
d  h=(i e=(c g)) k l)
```


### Stolen children
Consider this situation:

```
     r₁r₂    r₃     r₄
     | |     |      |
+----(-(-----(------(----+
V    V V     V      V    |
a=(b c d) e=(f g h) i=(j k l)
     ^    ^  |         |
     +----(--+         |
          |            |
          +------------+
```

_r₁_, _r₂_, and _r₃_ are false roots because `a` and `e` exist in the live set, which means the only true root is _r₄_. Thus the new heap would look like this:

```
r₄    r₃          r₁r₂
|     |           | |
V     V           V V
i=(e=(c g h) a=(b m d) l)
      ^           |
      +-----------+
```

This results in `a`, an unflagged container, having a stolen child, which means (1) it cannot be block-copied as a single `i9`, (2) it will be flagged in the new heap, and (3) its children have been relocated with respect to one another -- in particular `d`, which now exists at a different offset.

From an implementation perspective, this just means we need to trace-rewrite any `i9` whose old-space range contains any relocated objects; relocation means we must insert a reference.

Notice that _r₁_ and _r₂_ preserve their sibling relationship, and that _r₃_ is still the first child of `e`. This is intentional and a consequence of how the relocation table works: old-space `f` relocates to new-space `c`, and old-space `c` relocates to new-space `m` -- since these are the new objects that occur _at those locations within the copy chain_.

Because stolen children result in flagged containers, we end up creating a relocation table entry for each child of a container with stolen children. This means that although false roots may not all have relocation entries, their offsets with respect to those entries will remain constant.
