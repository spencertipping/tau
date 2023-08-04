# Spatial computation
τ provides spatial computation to navigate arbitrarily large spaces using a variety of different strategies. The basic constraints are these:

1. Space consists of an indeterminate set of nodes, each of which can be identified by an η value
2. A streaming function _f_ takes a node as input and returns traversal instructions, described in the next section
3. _f_ may request that other nodes be evaluated before it evaluates its own node
4. The set of to-be-evaluated nodes may be arbitrarily large, e.g. it may not fit into memory
5. _f_ may request that it be informed about progress at some future moment
6. At convergence, _f_ will assign each visited node a result value; this value is cached so _f_ is called at most once per node
7. _f_ is stateless and potentially asynchronous
8. To-be-evaluated nodes are prioritized, and _f_ will explore them in priority order


## _f_: the traversal function
If we have a process _n → f → I*_, where _I_ is an instruction, then `?f` should execute the program specified by _f_. That program may include:

+ _n ← n' p_: dependency on _f(n')_ with priority _p_
+ _n = v_: return value _v_ for node _n_
+ _t_: wake up at time _t_, even if not all deps are ready (_f_ will receive all dependencies with some mapped to _ω_)

**NOTE:** _I*_ is a horizonal η vector; that is, we emit _(n ← a) (n ← b)_ instead of _τ[n ← a, n ← b]_. We do this because _f_ may be internally parallel, e.g. with `PSN[...]`.

**NOTE:** _f_ must be stateless.

`?` manages several things:

1. Deduplicating _f(n')_ computations
2. Scheduling _f(n')_ calls, typically by priority
3. Caching _v ← f(n')_ results
4. Storing all result data

_f_ is invoked in one of the following ways:

+ _n → f_ (initial state)
+ _n (n₁ v₁) (n₂ v₂) ... → f_ (dependency results are available)

_n_ will be retired as soon as _f → n = v_; at that point `?` caches _n = v_.


## Parallelism
_f_ need not be synchronous; `?` may issue multiple inputs at once and _f_'s outputs for those inputs may be emitted out of order. That is, you can wrap any _f_ in `PS[]` and everything will still work normally, up to calculation ordering.

**TODO:** _f_ is idempotent, so `?` should be able to replay inputs if it doesn't get a result for some amount of time -- or if a parallel driver fails in some way


## `?` dataflow
`?` has only _f_ as its IO component; everything else is handled by working directly with the data backend, which is synchronous and not streaming.

+ _f → n=v ⇒ n α v → c; cf(n)_: store result, check for finalized dependents on _n_
+ _f → (n ← a) (n ← b) ⇒ [a ι → c] a α → q, [b ι → c] b α → q_: enqueue uncalculated results and reuse calculated ones
+ _f → ι ..._: emit _..._ directly to output stream
+ _f → t_: schedule a wakeup at time _t_, at which point _f_ will receive all currently-calculated results
+ _q → f_: _f_ gets work from the priority queue


## Cache update algorithm
```cpp
struct val
{
  int r;       // 0 = scheduled, 1 = waiting, 2 = resolved
  S<node> ps;  // nodes that depend on this one
  union { ηm v; V<node> ds; };
};

bool expired(node n)
{
  // We're expired if all parents are expired, or if this
  // node already has a value
  if (c[n].r == 2) return true;
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
  if (c[n].r != 1) return false;
  for (let d : c[n].ds)
    if (!c.contains(d)) return false;
  return true;
}

void resolve(node n, ηm v)
{
  if (c[n].r == 2) return;
  c[n].r = true;
  c[n].v = v;
  for (let p : c[n].ps)
    if (ready(p))
      rq.push(p);  // add p to the ready-queue
  c[n].ps.clear();
}

for (let x : fi)
{
  if (c[x.n].r == 2) continue;  // node is finalized
  if (x.is_deps())              // f → (n ← a) (n ← b) ...
    for (let y : x.ds)
    {
      c[x.n].ds.push_back(y.n);
      if (c[y.n].r != 2)
      {
        c[y.n].ps.insert(x.n);
        q.push(y.n);
      }
    }
  else if (x.is_neqv())         // f → n=v
    resolve(x.n, x.v);
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
          r << std::make_tuple(y, (c[y].r == 2 ? c[y].v : ω));
      });
```
