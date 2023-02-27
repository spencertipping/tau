# τ compiler structures
A **non-authoritative** summary of data structures, but enough to provide a sense for how these things work.


## Γ and Γ
```cpp
struct Γ
{
  τ             &t;
  V<Sp<Γ>>       g;     // pipeline of Γs, applied sequentially
  M<sym, Sp<ξ>> *w;     // named weak ξs (transient during operator())

  // Called from inside (), which populates *w and then erases it at the end
  Sp<ξ> r(Sp<ψ>, sym);  // grab the read-end of a named ξ
  Sp<ξ> w(Sp<ψ>, sym);  // grab the write-end of a named ξ

  // NOTE: w[] doesn't get populated until (), which effectively finalizes
  // the Γ
  Sp<Ξ> operator()(Ξ const&) const;
  Γ     operator| (Γ const&) const;
  Γ    &operator<<(Sp<Γ>);
  Γ    &operator>>(Sp<Γ>);
};

struct Γ
{
  virtual ~Γ() {}

  // Apply as a Ξ transform, with the ability to consult Γ for global ξs
  // ψ is created and referred to indirectly via the resulting Ξ
  virtual Sp<Ξ> operator()(Ξ const&, Γ&) const = 0;
};
```

The goal is to be able to implement `Γ::operator()` like this for an identity transform:

```cpp
struct Γi : public virtual Γ
{
  M<Ξi, ξm> i;  // input shaping matrix
  M<Ξi, ΞI> o;  // output shaping matrix

  Sp<Ξ> Γ::operator()(Ξ const &c, Γ &g) const
  {
    auto m = c(i, *g.w);  // materialize input ξs
    M<ΞI, Sp<ξ>> n;       // output vector
    for (let &[k, v] : o) n[v] = m[k];
    return c(i, n);
  }
};
```

Similarly, we can implement a single-ξ operator like this:

```cpp
struct Γsize : public virtual Γ
{
  Ξi i;
  ΞI o;

  Sp<Ξ> Γ::operator()(Ξ const &c, Γ &g) const
  {
    return c(i, o, (new ψsize(c(i, {false, false}, *g.w)))->o);
  }
};
```

A lot of this hinges on the Ξ interface, which I think can be fairly open-ended. Ξ is fully erased by pipeline runtime, so no need to optimize nor standardize it much.


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

  Ξ copy() const;   // shallow copy

  Sp<ξ> i()(Ξi, ξm)        const;  // input-side of a ξ
  Sp<ξ> o()(ΞI, ξm)        const;  // output-side of a ξ
  Sp<Ξ> x()(Ξi, ΞI, Sp<ξ>) const;  // replace a ξ

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
struct ξi { Sp<ξ> c; /* ... */ };
struct ξo { Wp<ξ> c; /* ... */ };

// Virtual base class for stream operators
struct ψ
{
  τ     &t;  // τ runtime for λ management
  S<λi>  l;  // managed λs
  virtual ~ψ() { for (let x : l) λx(x); }

  λi   λc(λf&&);
  void λx(λi);
};
```

Example single-stream operator:

```cpp
struct ψf : public virtual ψ
{
  ξi i;
  ξo o;
  F<void(ξi&, ξo&)> f;

  ψf(Sp<ξ> i_, Sp<ξ> o_, F<void(ξi&, ξo&) &&f_)
    : i(i_), o(o_), f(std::move(f_))
    { λc([&]() { f(i, o); }); }
};
```

Note that when ψ is deallocated, input ξs are also dereferenced, which decreases `shared_ptr` reference counts to writing ψs. This mechanism drives ψ GC.

**NOTE:** in practice, most ψ won't have `M<>` to track their ξs. Instead, they can just define instance variables -- the main thing is that we need reader-ξ and writer-ξ types to avoid any `Sp` vs `Wp` confusion.

**NOTE:** "closing" a ξ is just clearing its pointer; this has all of the right semantics, including the weak pointer being cleared automatically. So ξi and ξo can mediate the open/closed semantics for us, leaving just the queue as the remaining piece.
