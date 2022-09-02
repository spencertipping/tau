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
        return 0;
      }); }
};
```

Each operator has very little going on; it should be possible to do something like this instead:

```cpp
γ &iota(uN n)
{
  return Γ([n](γ &g, γφ, γφ o) {
    for (uN i = 0; i < n; ++i) o << i;
  });
}

γ &delay(ΔΘ dt)
{
  return Γ([dt](γ &g, γφ i, γφ o) {
    for (let x : i) g.Θ(dt), o << x;
  });
}
```


## Constraints
