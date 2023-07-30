# Spatial computation
Suppose _n_ is a node in space. A function _f(n)_ runs over a timeline _t(n) = t₀(n) → t₁(n)_ with node dependencies _n' ∈ N_. Here's what we know:

1. If _n' → n_, then _t₁(n) > t₁(n')_ (dependencies are used to compute result)
2. If _N ← n_ is the full set of nodes that depend on _n_, then _t₀(n) > t₀(n') ∀ n' ∈ N_ (nodes are not visited until something depends on them)
3. _f(n)_ is computed at most once for a given _n_ unless we're deliberately forgetful, so _t₀(n)_ and _t₁(n)_ are both well-defined
4. (1) implies that the order in which dependencies are evaluated does not matter

**Deadline search:** _n ← (n', t, v)_ evaluate _n'_ for _n_ before time _t_; if no result is found, return _v_

Deadlines are prioritized in a queue per worker.


## Original thoughts
The goal of any `?` operator is to handle topology and eliminate loops. So we should be able to define functionality in a "stream of node visits → stream of next steps" kind of way. Some next steps for a node _n_:

+ Emit _n_ with score _s_
+ Terminate at _n_
+ Enqueue _n'_ on frontier with priority _p_
+ Define dependency _n → n'_
+ Define result/attribute for _n_ (we need this to define dependencies)
+ Cache return value for _n_ (or maybe request caching for _n'_)

**Q:** how do we specify what to do with dependency data? For example, `du` involves summing the child sizes; so we might stream out _n → n'_ for each child, then _v ← +/x_ or something.

**NOTE:** node attributes need not exist; we can use node values and multiplex on the nodes themselves (since nodes can be η).

**NOTE:** dependencies must be emitted in a complete list; that way we'll know when we have all _n' → n_ and can generate _n_'s value

It is very appropriate to use π to calculate a search derivative, then have `?` integrate it. Derivatives are time-invariant in a way that integrals are not.

**NOTE:** `? cback` is slightly wrong because we'll have a k/v cache and a queue -- `cback` doesn't give us space for both (unless we use `N`, which is implicitly unshared). We can fix this by defining a sub-namespacing function for `cback` that allows us to append something to the table/DB name.

**Q:** are there situations where we need asynchronous neighbor generation? If so, we really have _n → τ[Δ]_ where _Δ_ is the search-instruction stuff.


## Linearizing spatial domains
First, we don't technically linearize the domain if parallelism is involved. Our events are partially ordered with nondeterminism if multiple branches choose to terminate at once. So **termination is a per-branch criterion.**

With that in mind, a domain is linearized by constructing a spatial path and tying it to a timeline. This means we have two options:

1. Emit fully-ordered events
2. Branch into multiple parallel timelines
