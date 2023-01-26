# τ compiler structures
A **non-authoritative** summary of data structures, but enough to provide a sense for how these things work.


## Γ and γ
```cpp
struct Γ
{
  M<sym, Sp<ξ>> w;      // named weak ξs
  V<Sp<γ>>      g;      // pipeline of γs

  Sp<ξ> r(Sp<ψ>, sym);  // grab the read-end of a named ξ
  Sp<ξ> w(Sp<ψ>, sym);  // grab the write-end of a named ξ

  // NOTE: w[] doesn't get populated until (), which effectively finalizes
  // the Γ
  Sp<Ξ> operator()(Ξ const&) const;
  Γ     operator| (Γ const&) const;
  Γ    &operator<<(Sp<γ>);
  Γ    &operator>>(Sp<γ>);
};

struct γ
{
  M<Ξi, ξm> i;  // input vector profile (rows)
  M<ΞI, ξm> o;  // output vector profile (columns)

  // Apply as a Ξ transform, with the ability to consult Γ for global ξs
  // ψ is created and referred to indirectly via the resulting Ξ
  virtual Sp<Ξ> operator()(Ξ const&, Γ&) const = 0;
};
```


## Ξ runtime
```cpp
struct ξ
{
  Wp<ψ> r;  // weak ref to reader
  ζ     p;  // η value pipe
  virtual ~ξ() {}
};
struct ξs : public virtual ξ { Sp<ψ> w; };  // strong ξ
struct ξw : public virtual ξ { Wp<ψ> w; };  // weak ξ

typedef V<sym> Ξi;  // Ξ → ξ
typedef V<sym> ΞI;  // ξ → Ξ
struct ξm           // ξ metadata
{
  bool i;           // if true, data moves right to left
  bool w;           // if true, ξ weakly references its writer
};

struct Ξ            // Ξ vector
{
  M<sym, Sp<Ξ>> f;  // sub-Ξs
  M<sym, Sp<ξ>> c;  // materialized ξ

  // Extract ξs from input Ξ, using a mixture of Ξ-provided ξs and
  // ones provided via global map
  M<Ξi, Sp<ξ>> operator()(Sp<ψ>, Mc<Ξi, ξm>&, Mc<sym, Sp<ξ>>&) const;

  // Construct output Ξ from this, input, and newly-assigned outputs
  Sp<Ξ>        operator()(Sp<ψ>, Mc<Ξi, ξm>&, Mc<ΞI,  Sp<ξ>>&) const;
};
```

**NOTE:** a weak ξ is constructed if _either_ endpoint requests weakness.


## ψ runtime
```cpp
// Virtual base class for stream operators
struct ψ
{
  τ           &t;  // τ runtime for λ management
  S<λi>        l;  // managed λs
  M<Ξi, Sp<ξ>> i;  // active input ξs (strong references)
  M<Ξi, Wp<ξ>> o;  // active output ξs (weak references)
  virtual ~ψ() { for (let x : l) t.λx(x); }
};
```

Note that when ψ is deallocated, input ξs are also dereferenced, which decreases `shared_ptr` reference counts to writing ψs. This mechanism drives ψ GC.
