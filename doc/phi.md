# Φ
Φ is the smallest unit of atomicity we have; as a boundary, everything inside Φ is guaranteed to (1) operate on a single, totally-ordered timeline; (2) have process-persistent heap storage; and (3) be atomically instantiable by the flux core (Γ, via Λ).

Φ is also the target of the user-facing side of τ: programs are written as a flux-composed collection of Φ cells.

This means operators are inside-Φ multi-coro actors, programs are outside-Φ flux topologies.

γs don't refer to Φ directly.


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
