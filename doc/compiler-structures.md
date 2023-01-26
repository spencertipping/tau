# τ compiler structures
A **non-authoritative** summary of data structures, but enough to provide a sense for how these things work.


## Γ and γ
```cpp
struct Γ
{
  τ             &t;
  M<sym, Sp<ξ>> *w;     // named weak ξs
  V<Sp<γ>>       g;     // pipeline of γs

  // Called from inside (), which populates *w and then erases it at the end
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
  // Apply as a Ξ transform, with the ability to consult Γ for global ξs
  // ψ is created and referred to indirectly via the resulting Ξ
  virtual Sp<Ξ> operator()(Ξ const&, Γ&) const = 0;
};
```

The goal is to be able to implement `γ::operator()` like this for an identity transform:

```cpp
struct γi : public virtual γ
{
  M<Ξi, ξm> i;  // input shaping matrix
  M<Ξi, ΞI> o;  // output shaping matrix

  Sp<Ξ> γ::operator()(Ξ const &c, Γ &g) const
  {
    auto m = c(i);   // materialize input ξs
    M<ΞI, Sp<ξ>> n;  // output vector
    for (let &[k, v] : o) n[v] = m[k];
    return c(i, n);
  }
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
  // ones provided via global map; ψ is set as the ξ endpoint
  M<Ξi, Sp<ξ>> operator()(Sp<ψ>, Mc<Ξi, ξm>&, Mc<sym, Sp<ξ>>&) const;
  M<Ξi, Sp<ξ>> operator()(       Mc<Ξi, ξm>&, Mc<sym, Sp<ξ>>&) const;
  M<Ξi, Sp<ξ>> operator()(       Mc<Ξi, ξm>&                 ) const;

  // Construct output Ξ from this, input, and newly-assigned outputs
  // (inputs are erased, outputs are added); ψ is set as the ξ endpoint
  Sp<Ξ>        operator()(Sp<ψ>, Mc<Ξi, ξm>&, Mc<ΞI,  Sp<ξ>>&) const;
  Sp<Ξ>        operator()(       Mc<Ξi, ξm>&, Mc<ΞI,  Sp<ξ>>&) const;
};
```

**NOTE:** a weak ξ is constructed if _either_ endpoint requests weakness.

**Q:** what's the point of Ξ nesting if we have no convenient mechanism to un-nest groups of ξs?


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
