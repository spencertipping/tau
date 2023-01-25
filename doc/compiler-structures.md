# τ compiler structures
A **non-authoritative** summary of data structures, but enough to provide a sense for how these things work.


## Γ and γ
```cpp
struct Γ
{
  M<sym, ξ> w;       // named weak ξs
  V<γ>      g;       // pipeline of γs

  // TODO: how do we work named ξs into the intermediate φs in the pipeline?
  // NOTE: it may help to define disjoint symbol domains, one for φ keys and
  // one for Γ-level cyclic refs
};

struct γ
{
  M<φi, ξm> i;       // input vector profile (rows)
  M<φI, ξm> o;       // output vector profile (columns)
  ψ operator()(φ&);  // apply to φ to create resources
};
```


## φ runtime
```cpp
struct ξ
{
  Sp<ψ> ws;  // strong ref to writer
  Wp<ψ> ww;  // weakref to writer
  Wp<ψ> r;   // weakref to reader
  ζ     p;   // η value pipe
};

typedef V<sym> φi;  // φ → ξ
typedef V<sym> φI;  // ξ → φ
struct ξm           // ξ metadata
{
  bool i;           // if true, data moves right to left
  bool w;           // if true, ξ weakly references its writer
};

struct φ            // φ vector
{
  M<sym, Sp<φ>> f;  // sub-φs
  M<sym, Up<ξ>> c;  // materialized ξ
  M<φi, ξ> operator()(M<φi, ξm>    const&);
  Sp<φ>    operator()(M<φI, Up<ξ>> const&);
};
```


## ψ runtime
```cpp
struct ψ;
typedef F<void(ψ&)> ψc;  // ψ incremental constructor
typedef F<void(ψ&)> ψd;  // ψ incremental destructor
struct ψ
{
  V<ψc> c;
  V<ψd> d;
  S<λi> l;  // active λs
  φ     i;  // input φ
  φ     o;  // output φ
  ~ψ() { for (let f : d) f(*this); }
};
```

Note that when ψ is deallocated, it destroys its input and output φs, each of which destroy their ξs, which decreases `shared_ptr` reference counts to writing ψs. This mechanism drives ψ GC.
