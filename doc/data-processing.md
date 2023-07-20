# Data processing applications in τ
τ and `ni` use the same patterns to process data: apply a [Γ](Gamma.md) to a stdin/stdout [Ξ](Xi.md) and execute against some multiplexing backend (for `ni`, the kernel; for τ, [τ](tau.md)).


## Incremental search/traversal
The goal is to generalize breadth-first, depth-first, and prioritized searches in such a way that we can operate incrementally. The structure of the search is determined by the dependencies between nodes; for example, `du` favors depth-first because subdirectories contribute to their parent. So we're automating both _traversal_ (dependency-directed) and _search_ (possibility-directed, sometimes with optimality weights). In each case we can store the results in a cache to make the process fully resumable.


### Core compute logic
At its core, search involves visiting nodes and inspecting their dependencies and neighbors. So we have these functions:

+ _D(n) → τ[n₁, n₂, ...]_: the τ-delimited list of nodes _n_ depends on
+ _A(n) → τ[n₁, n₂, ...]_: the τ-delimited list of nodes adjacent to _n_
+ _P(n)_: the priority of a node
+ _R : τ[n, (n₁, r₁), (n₂, r₂), ...] → r_: the result value for a node and its dependencies

If we're searching, then we have _P(n)_ to determine the priority of a node. The guarantee is that results from _A_ will always be explored in _P_-descending order. This can serve as a type of pruning, especially if we use a bounded priority queue.


### Caching redundant spaces
Let's imagine we want to traverse each unique entry within a ZFS-style filesystem (to calculate the on-disk size, for example), wherein each directory is immutable but may be referenced by multiple snapshots or other directories. We want to expand each unique directory at most once since there may be many snapshots.

We begin by enumerating each snapshot root as a node; this is our initial frontier. Each contains its children as dependencies. Immediately we run into the expand-once constraint: by default, this traversal will probably produce multiple visits to the same directory at the same revision. So we introduce two caches:

+ _C₁(n)_: tracks whether _n_ has been entered into any work queue
+ _C₂(n, r)_: the cached result value for _n_ and its dependencies, if it is known

_C₁_ is necessary because we may visit a node multiple times before any of its results have been calculated: we must reject all expansions resulting from all beyond the first visit.


### τ/Γ topology
In the most general case, `?G[C₁][C₂][Q][D][A][P][R]` is a Ψ₂ that consumes τ-delimited nodes to explore and emits co-τ-delimited _(n', r)_ pairs of results as they are computed. Note that _n ⊆ n'_ here, since _D_ and _A_ will probably expand the set.

First the data-management components:

+ _C₁_ is a filter: it accepts every _n_ being visited, but emits only _n_ that have not been seen before. _τ_ is forwarded from `?G` input and should reset the cache.
+ _C₂_ is a filter that takes _n_ and emits _(n, r)_ or _(n, ω)_. _τ_ resets it.
+ _Q_ is the queue: it should take _(p, n)_ and emit one _(p, n)_ in _p_-descending order per _ι_ signal. _τ_ clears it.

Search components are stateless and work as described above:

+ _D : n → τ[n']_
+ _A : n → τ[n']_
+ _P : n → p_ (this is a π function, not a Γ)
+ _R : τ[n, (n₁, r₁), ...] → r_


### `?` IO
`?` has a simple interface: initial nodes _n_ go in, _(n', r)_ come out, and _τ_ signals are propagated. However, some use cases are too complex for Ψ₂; for example, suppose we want the first _k_ results from a given search, or we want to produce results until one satisfies a predicate.

**FIXME:** Ψ₂ is insufficient for the above case; we can:

1. Introduce a _T_ terminator Γ
2. Define `?` as a Ψ₄, with right-loop gating (basically, _T_ as right loop)
3. Use input _ι_ signaling to prompt for results

**NOTE:** we'll usually want one of these:

1. The first result
2. The first _k_ results
3. Results until _p(n, r)_ is true
4. All results

We don't need to manage multiple unrelated searches with tight latency bounds, so it should be fine to default to one of these behaviors.


### `?` shorthands
Not every search requires all components. Depth-first is considerably simpler, omitting `Q`, `A`, and `P`:

```
?D[C₁][C₂][D][R]
```
