# Spatial computation
Suppose _n_ is a node in space. A function _f(n)_ runs over a timeline _t(n) = t₀(n) → t₁(n)_ with node dependencies _n' ∈ N_. Here's what we know:

1. If _n' → n_, then _t₁(n) > t₁(n')_ (dependencies are used to compute result)
2. If _N ← n_ is the full set of nodes that depend on _n_, then _t₀(n) > t₀(n') ∀ n' ∈ N_ (nodes are not visited until something depends on them)
3. _f(n)_ is computed at most once for a given _n_ unless we're deliberately forgetful, so _t₀(n)_ and _t₁(n)_ are both well-defined
4. (1) implies that the order in which dependencies are evaluated does not matter

**Deadline search:** _n ← (n', t, v)_ evaluate _n'_ for _n_ before time _t_; if no result is found, return _v_

Deadlines are prioritized in a queue per worker.

Ok, suppose we have multiple timeline points for a given node -- so _(n', v)_ comes back and we submit more _n''_ for evaluation, potentially with a new deadline.


## `?` core
If we have a process _n → f → I*_, where _I_ is an instruction, then `?f` should execute the program specified by _f_. That program may include:

+ _n ← n'_: unbounded dependency on _f(n')_
+ _n ← n' t v_: time-bounded dependency on _f(n')_, defaulting to _v_
+ _n = v_: return value _v_ for node _n_

**NOTE:** _I*_ is a horizonal η vector; that is, we emit _(n ← a) (n ← b)_ instead of _τ[n ← a, n ← b]_. We do this because _f_ may be internally parallel, e.g. with `PSN[...]`.

**NOTE:** _f_ must be stateless.

`?` manages several things:

1. Deduplicating _f(n')_ computations
2. Scheduling _f(n')_ calls, typically by nearest-deadline-first (using `@<`)
3. Caching _v ← f(n')_ results
4. Storing all result data in a `@:` container

_f_ is invoked in one of the following ways:

+ _n → f_ (initial state)
+ _n n₁ v₁ n₂ v₂ ... → f_ (dependency results are available)

_n_ will be retired as soon as _f → n = v_; at that point `?` caches _n = v_.


## Parallelism
_f_ need not be synchronous; `?` may issue multiple inputs at once and _f_'s outputs for those inputs may be emitted out of order. That is, you can wrap any _f_ in `PS[]` and everything will still work normally, up to calculation ordering.
