# τ compiler structures
A **non-authoritative** summary of data structures, but enough to provide a sense for how these things work.


## Γ and γ
**FIXME:** we can't name-insert within Γ like is implied by the API; we need to have custom γs that do this name-grabbing perhaps. Right now it's unclear how we grab ξs from `w`.

```cpp
struct Γ
{
  M<sym, Sp<ξ>> w;   // named weak ξs
  V<γ>          g;   // pipeline of γs
};

struct γ
{
  M<Ξi, ξm> i;                // input vector profile (rows)
  M<ΞI, ξm> o;                // output vector profile (columns)
  Sp<Ξ> operator()(Ξ&, ψ**);  // apply to Ξ to create resources
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
struct ξs : public virtual ξ { Sp<ψ> w; };
struct ξw : public virtual ξ { Wp<ψ> w; };

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
  M<Ξi, Sp<ξ>> operator()(M<Ξi, ξm> const&, M<sym, Sp<ξ>> const&) const;

  // Construct output Ξ from this, input, and newly-assigned outputs
  Sp<Ξ> operator()(M<Ξi, ξm> const&, M<ΞI, Sp<ξ>> const&) const;
};
```

**NOTE:** a weak ξ is constructed if _either_ endpoint requests weakness.


## ψ runtime
```cpp
struct ψ;
typedef F<void(ψ&)> ψc;  // ψ incremental constructor
typedef F<void(ψ&)> ψd;  // ψ incremental destructor

struct ψ
{
  V<ψc>        c;
  V<ψd>        d;
  S<λi>        l;  // managed λs
  M<Ξi, Sp<ξ>> i;  // active input ξs
  M<Ξi, Wp<ξ>> o;  // output ξs (note weak reference)
  ~ψ() { for (let f : d) f(*this); }
};
```

Note that when ψ is deallocated, input ξs are also dereferenced, which decreases `shared_ptr` reference counts to writing ψs. This mechanism drives ψ GC.
