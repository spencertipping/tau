# Γ
The τ flux core (Γ) moves data through pipes (Ψ), scheduling coros (Λ) and managing time (Θ) in the process. Materially, Γ is a single UNIX process or WASM thread that uses a single address space, and typically runs an `epoll` or browser event loop.

Γ guarantees a unified, fully-ordered timeline for all λ running within it. Events across ζ/ψ within a single Γ are guaranteed to serialize deterministically, even across multiple λ and γ. The flux IO boundary is also guaranteed to provide a unified view of the outside world.


## Inner (operator) API
Operators receive a γ object that functions as their Γ call interface.

```cpp
struct γ
{
  u9      gs();            // gensym for identifiers
  ΣΘΔ     γΘ() const;      // Σ[λΘ]

  γ      &λc(u9, λf);
  bool    λe(u9) const;    // does λ exist
  λr      λw(u9);          // gracefully await λ
  γ      &λx(u9);          // kill λ
  ΣΘΔ    &λΘ(u9) const;

  bool    ψri(u9);         // is readable right now
  bool    ψrw(u9);         // is readable
  u9      ψr (u9);         // read item
  bool    ψwi(u9) const;   // is writable right now
  bool    ψww(u9) const;   // is writable
  bool    ψw (u9, u9c &);  // write item
  bool    ψe (u9) const;   // does ψ exist
  Ψ::ψit  ψs (u9) const;   // iterable

  γ      &Θw(ΔΘ s) { return Θw(now() + s); }
  γ      &Θw(Θp);
};
```

**NOTE:** `ψw` has the option of just `obuf << v` for memory management, which is worst-case about as computationally expensive as data structure allocation in Python. Then the underlying pipe carries `shared_ptr<obuf>` objects and we have no issues with GC. If we have exceptionally large values, we can refer to them using a static checkin/checkout dictionary Φ -- but that requires us to manage GC manually.
