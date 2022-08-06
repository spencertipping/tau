# Φ
Φ is the smallest unit of atomicity we have; as a boundary, everything inside Φ is guaranteed to (1) operate on a single, totally-ordered timeline; (2) have process-persistent heap storage; and (3) be atomically instantiable by the flux core (Γ, via Λ).

Φ is also the target of the user-facing side of τ: programs are written as a flux-composed collection of Φ cells.

This means operators are inside-Φ multi-coro actors, programs are outside-Φ flux topologies.


## Inner abstraction
The API used to write an operator within the τ runtime.

```cpp
struct Φ::inner
{
  u9        gs();                // gensym for identifiers
  ΣΘΔ       γΘ() const;          // Σλ

  Φ::inner &λc(u9, λf);
  bool      λe(u9) const;        // does λ exist
  λr        λw(u9);              // gracefully await λ
  Φ::inner &λx(u9);              // kill λ
  ΣΘΔ       λΘ(u9) const;

  Φ::inner &ψc(u9);              // create port (send ω to close)
  bool      ψe(u9) const;        // does ψ exist
  u9        ψr(u9);              // read item
  bool      ψri(u9) const;       // is readable right now
  bool      ψni(u9);             // has next item
  bool      ψw(u9, u9 const &);  // write item
  bool      ψwi(u9) const;       // is writable right now
  Ψ::ψit    ψs(u9) const;        // iterable

  Φ::inner &Θa(ΔΘ s) { return Φa(now() + s); }
  Φ::inner &Θa(ΣΘΔ::Θp);
};
```

**NOTE:** `ψw` has the option of just `obuf << v` for memory management, which is worst-case about as computationally expensive as data structure allocation in Python. Then the underlying pipe carries `shared_ptr<obuf>` objects and we have no issues with GC. If we have exceptionally large values, we can refer to them using a static checkin/checkout dictionary Φ -- but that requires us to manage GC manually.


## Outer abstraction
The API used by Γ to manage individual operators within the larger flux context.

λs are managed by Λ independently of γ, so we don't need to treat them specially here.

```cpp
struct Φ::outer
{
  Γ        &c;
  λi        λp;  // primary coro (γ implodes when this returns)
  M<u9, ψi> ψs;  // local UTF9 symbol → ID lookup
  M<u9, λi> λs;

  Φ::outer &ψc(u9, ψi);
  Φ::outer &λc(u9, λi);
  Φ::outer &ψx(u9 const &);
  Φ::outer &λx(u9 const &);

  ΣΘΔ       γΘ() const;  // time profile
};
```
