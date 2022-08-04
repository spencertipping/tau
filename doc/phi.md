# Φ
Φ is the smallest unit of atomicity we have; as a boundary, everything inside Φ is guaranteed to (1) operate on a single, totally-ordered timeline; (2) have process-persistent heap storage; and (3) be atomically instantiable by the flux core (Γ, via Λ).

Φ is also the target of the user-facing side of τ: programs are written as a flux-composed collection of Φ cells.

This means operators are inside-Φ multi-coro actors, programs are outside-Φ flux topologies.


## Inner abstraction
The API used to write an operator within the τ runtime.

```cpp
struct Φ::inner
{
  Φ::inner &λc(u9, λf);
  λr        λw(u9);
  Φ::inner &λx(u9);

  Φ::inner &ψc(u9);        // send ω to close+delete a port
  u9        ψr(ψi);
  bool      ψa(ψi);
  bool     &ψw(ψi, u9 const&);
  Ψ::ψit    ψs(ψi) const;  // iterable

  Φ::inner &Θa(Θ::span s) { return Φa(now() + s); }
  Φ::inner &Θa(Θ::point);
};
```

## Outer abstraction
The API used by Γ to manage individual operators within the larger flux context.

```cpp
struct Φ::outer
{
  Γ &c;
  λi        λp;  // primary coro (operator implodes when this returns)
  M<u9, ψi> ψm;  // local UTF9 symbol → ID lookup
  M<u9, λi> λm;
};
```
