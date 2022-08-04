# Φ
Φ is the smallest unit of atomicity we have; as a boundary, everything inside Φ is guaranteed to (1) operate on a single, totally-ordered timeline; (2) have process-persistent heap storage; and (3) be atomically instantiable by the flux core (Γ, via Λ).

Φ is also the target of the user-facing side of τ: programs are written as a flux-composed collection of Φ cells.

This means operators are inside-Φ multi-coro actors, programs are outside-Φ flux topologies.


## Inner abstraction
The API used to write an operator within the τ runtime.

```cpp
struct Φ::inner
{
  u9        gs();          // gensym for identifiers
  Θw        p() const;     // profile for this γ (across all coros)

  Φ::inner &λc(u9, λf);
  bool      λe(u9) const;
  λr        λw(u9);
  Φ::inner &λx(u9);
  Θw        λp(u9) const;  // profile for a coro

  Φ::inner &ψc(u9);        // create port (send ω to close)
  bool      ψe(u9) const;
  u9        ψr(u9);
  bool      ψa(u9);
  bool      ψw(u9, u9 const&);
  Ψ::ψit    ψs(u9) const;  // iterable

  Φ::inner &Θa(Θ::span s) { return Φa(now() + s); }
  Φ::inner &Θa(Θ::point);
};
```

**TODO:** `ψw` needs some GC attention


## Outer abstraction
The API used by Γ to manage individual operators within the larger flux context.

λs are managed by Λ independently of γ, so we don't need to treat them specially here.

```cpp
struct Φ::outer
{
  Γ        &c;
  λi        λp;  // primary coro (γ implodes when this returns)
  M<u9, ψi> ψm;  // local UTF9 symbol → ID lookup
  M<u9, λi> λm;

  Φ::outer &ψc(u9, ψi);
  Φ::outer &λc(u9, λi);
  ψi        ψg(u9 s) const { return ψm.at(s); }
  λi        λg(u9 s) const { return λm.at(s); }
  Φ::outer &ψd(u9);
  Φ::outer &λd(u9);

  Θs        Θp() const;  // time profile
};
```
