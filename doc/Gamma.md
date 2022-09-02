# Γ
A DSL that simplifies the process of creating [γ](gamma.md) components.


## Background
Right now I write γ components like this:

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


## Structure
+ One or more λs
+ Init args, copied in by value
+ Runtime storage shared across λs

Also worth considering: we may want some type of unified structure to initialize, rather than having open-ended C++. For example, we might insist on `i9`. We also want to store these things in a dispatch table rather than defining C++ symbols directly.

```cpp
Γ(iota,
  uN n = Ξ(n, -1);
  for (uN i = 0; i < n; ++i) o << i; )
```

The main λ can create others with `g.λc()`, and it can manage memory and create any local heaps that might be needed. `Ξ` is a preprocessor macro that expands to code that accesses specific configuration options. If the main λ exits, then the γ is automatically destroyed.
