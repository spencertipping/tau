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
