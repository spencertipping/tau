# γ authoring
How we write C++-hosted primitives.


## Low-level API
With no abstraction, ⌊γ components would be written like this:

```cpp
struct iota
{
  γ  g;
  uN n;
  iota(Φ &f, uN n_ = -1) : g(f), n(n_)
    { g.λc([&]() {
        for (uN i = 0; i < n; ++i)
          g.ο() << i;
        g.ο().ω();  // very important (though it shouldn't be)
        return 0;
      }); }
};

struct delay
{
  γ  g;
  ΔΘ dt;
  delay(Φ &f, ΔΘ dt_) : g(f), dt(dt_)
    { g.λc([&]() {
        for (let x : g.ι()) g.Θ(dt).ο() << x;
        g.ο().ω();
        return 0;
      }); }
};
```

Each operator has very little going on; it should be possible to do something like this instead:

```cpp
γ &iota(uN n)
{ return Γ([n](γ &g, γφ, γφ o) {
    for (uN i = 0; i < n; ++i) o << i;
    // all ports auto-closed when this λ returns
  }); }

γ &delay(ΔΘ dt)
{ return Γ([dt](γ &g, γφ i, γφ o) {
    for (let x : i) g.Θ(dt), o << x;
  }); }
```

It's probably worth some iteration to figure this out.


## Structure
+ One or more λs
+ Init arg, copied in by value (toplevel k/v `i9`)
+ Runtime storage shared across λs
+ **TODO later:** specify a micro-heap for each γ, to avoid `new`/`delete` overhead

```cpp
defγ(iota) {
  uN n = Ξ("n", -1);
  for (uN i = 0; i < n; ++i) ο << i;
};
```

**TODO:** declare Ξ interfacing in a way that informs its τ-source DSL integration -- more general than parser combinators, but more specific than "here's an option with no further context"

**TODO:** Ξ should use an enum or other hard type, not strings, to select data entries

The main λ can create others with `g.λc()`, and it can manage memory and create any local heaps that might be needed. `Ξ` accesses individual config options. If the main λ exits, then the γ is automatically destroyed (which closes its φs).

γ also converts the `return n` interface to a more UNIX-like exit-from-anywhere setup. Any of the participating λs can invoke `g.x()` to implode the γ, cleaning up all resources and (**TBD**) setting the final state of the γ, which can be observed by whoever created it.


## γ interfacing
1. How are γs created and later identified?
2. How does a γ exit normally vs abnormally, if at all?
3. If we have compound/nested γs, who manages the compound linkages?
4. For γs that have multiple φs, how are those φs declared before γ₀ creates the ζ linkages?


## γ₀
First, each τ program has a bootup γ, γ₀, that accepts further UTF9-encoded instructions to create γs and connect them. Each connection request consists of two pieces:

1. A map of `γid → (name config-map)` defining the γs we intend to create
2. A list of `(γid1, φ1, γid2, φ2)` tuples defining the space of ζs

Addresses are unique to a specific connection request. The γ₀ will reply with another mapping of `γid → (Γid = (Φid, Φγid))` that anchors each request-local γ ID to a permanent and globally unique one.
