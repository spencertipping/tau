# Incremental search/traversal
The goal is to generalize breadth-first, depth-first, and prioritized searches in such a way that we can operate incrementally. The structure of the search is determined by the dependencies between nodes; for example, `du` favors depth-first because subdirectories contribute to their parent. So we're automating both _traversal_ (dependency-directed) and _search_ (possibility-directed, sometimes with optimality weights). In each case we can store the results in a cache to make the process fully resumable.


## Core compute logic
At its core, search involves visiting nodes and inspecting their dependencies and neighbors. So we have these functions:

+ _D(n) → τ[n₁, n₂, ...]_: the τ-delimited list of nodes _n_ depends on
+ _A(n) → τ[n₁, n₂, ...]_: the τ-delimited list of nodes adjacent to _n_
+ _P(n)_: the priority of a node
+ _R : τ[n, (n₁, r₁), (n₂, r₂), ...] → r_: the result value for a node and its dependencies

If we're searching, then we have _P(n)_ to determine the priority of a node. The guarantee is that results from _A_ will always be explored in _P_-descending order. This can serve as a type of pruning, especially if we use a bounded priority queue.


## Caching redundant spaces
Let's imagine we want to traverse each unique entry within a ZFS-style filesystem (to calculate the on-disk size, for example), wherein each directory is immutable but may be referenced by multiple snapshots or other directories. We want to expand each unique directory at most once since there may be many snapshots.

We begin by enumerating each snapshot root as a node; this is our initial frontier. Each contains its children as dependencies. Immediately we run into the expand-once constraint: by default, this traversal will probably produce multiple visits to the same directory at the same revision. So we introduce two caches:

+ _C₁(n)_: tracks whether _n_ has been entered into any work queue
+ _C₂(n, r)_: the cached result value for _n_ and its dependencies, if it is known

_C₁_ is necessary because we may visit a node multiple times before any of its results have been calculated: we must reject all expansions resulting from all beyond the first visit.


## τ/Γ topology
In the most general case, `?C₁C₂QDAPRT` is a Ψ₂ that consumes τ-delimited nodes to explore and emits co-τ-delimited _(n', r)_ pairs of results as they are computed. Note that _n ⊆ n'_ here, since _D_ and _A_ will probably expand the set.

First the data-management components:

+ _C₁ : Ψ₂_ is a filter: it accepts every _n_ being visited, but emits only _n_ that have not been seen before. _τ_ is forwarded from `?` input and should reset the cache.
+ _C₂ : Ψ₂_ is a filter that takes _(n, r) → ι_ or _n → r | τ_ . Inbound _τ_ resets it.
+ _Q : Ψ₂_ is the queue: it should take _(p, n)_ and emit one _(p, n)_ in _p_-descending order per _ι_ signal. _τ_ clears it.
+ _T : Ψ₂_ is the terminal output filter: it takes _(n, r)_ and returns true or false to indicate whether more results should be generated. _τ_ resets it.

Search components are stateless -- i.e. they don't interact with input _τ_ -- and work as described above:

+ _D : Ψ₂ :: n → τ[n']_
+ _A : Ψ₂ :: n → τ[n']_
+ _P : Ψ₂ :: n → p_
+ _R : Ψ₂ :: τ[n, (n₁, r₁), ...] → r_


## Caching and databases
_C₁_, _C₂_, and _Q_ all use standard protocols that are supported by `@` and `Q`. For example, to use SQLite as the node cache and a C++ in-memory priority queue for _Q_:

```
?[@?S"cache.db:c1"]  # @Su = sqlite as set
 [@:S"cache.db:c2"]  # @S  = sqlite as map
 [Q>]                # Q>  = max-priority queue
 ...
```

Note that it isn't a problem to refer to `cache.db` multiple times; this is managed at the [τ](tau.md) level and will reuse the same connection.

**TODO:** verify that the above _C₂_ and _Q_ protocols make sense; we may want to modify them for more generality along the `@` and `Q` ends


## Shorthands
**TODO:** start with examples and generalize from there
