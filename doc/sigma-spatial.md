# Spatial computation
If we have a process _n → f → I*_, where _I_ is an instruction, then `?f` should execute the program specified by _f_. That program may include:

+ _n ← n'_: unbounded dependency on _f(n')_
+ _n = v_: return value _v_ for node _n_
+ _t_: wake up at time _t_, even if not all deps are ready

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


## `?` dataflow
**NOTE:** remove time-deadlines; we should instead have wakeup requests

+ _n → ?[n ι → c] → q_: enqueue frontier
+ _f → n=v ⇒ n α v → c; cf(n)_: store result, check for finalized dependents on _n_
+ _f → (n ← a) (n ← b) ⇒ [a ι → c] a α → q, [b ι → c] b α → q_: enqueue uncalculated results and reuse calculated ones
+ _f → (n ← a t v₁) (n ← b t v₂) ⇒ ..._: same, but enqueue with _t_ as priority
+ _q [t < θ] → f_: _f_ gets work from the queue if it hasn't expired

**NOTE:** there is no need to prune intermediate results; _f_ is invoked only when all of its dependents have values, whether calculated or failover. If _n' t ← n''_, then we automatically assign _t_ to _n''_ so it is terminated at the same time. (**NOTE:** this means _f_ must receive _n t_, not just _n_.)

**NOTE:** `?` can negotiate one-at-a-time IO by explicitly `λy` after each entry, then awaiting `ra() == 0` on the outbound ξ. This guarantees that we don't buffer incorrectly-prioritized work.


## Cache update algorithm
```cpp
struct val
{
  // FIXME: r should be a tri-state: scheduled/deps/resolved
  bool r;      // true if resolved
  S<node> ps;  // nodes that depend on this one
  union { ηm v; V<node> ds; };
};

bool expired(node n)
{
  // We're expired if all parents are expired, or if this
  // node already has a value
  if (c[n].r) return true;
  for (let p : n.ps)
    if (!expired(n.p))
      return false;
  return true;
}

for (let x : q)
  // Send all non-expired nodes to f
  if (!expired(x) && !s.contains(x))
  {
    s.insert(x);  // mark x as scheduled
    fo.r() << x;
  }
```

Then we have _f_'s output:

```cpp
bool ready(node n)
{
  if (c[n].r) return false;  // done, not ready
  for (let d : c[n].ds)
    if (!c.contains(d)) return false;
  return true;
}

void resolve(node n, ηm v)
{
  if (c[n].r) return;
  c[n].r = true;
  c[n].v = v;
  V<node> rs;  // to remove
  for (let p : c[n].ps)
    if (ready(p))
      rq.push(p);  // add p to the ready-queue
  c[n].ps.clear();
}

for (let x : fi)
{
  if (c[x.n].r) continue;  // node is finalized
  if (x.is_deps())         // f → (n ← a) (n ← b) ...
    for (let y : x.ds)
    {
      c[y.n].ps.insert(x.n);
      c[x.n].ds.push_back(y.n);
      q.push(y.n);
    }
  else if (x.is_neqv())    // f → n=v
  {
    resolve(x.n, x.v);
  }
}
```

The final piece activates _f_ from the ready queue:

```cpp
for (let x : rq)
  if (!expired(x))
    fo.r([&](auto &&r)
      {
        r << x;
        for (let y : c[x].ds)
          r << y << (c[y].r ? c[y].v : ω);
      });
```
