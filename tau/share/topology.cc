#include "topology.hh"
#include "../begin.hh"

namespace τ
{


typedef F<void(Sp<ψ>, ξi, ξo)> ffn;
typedef F<void(Sp<ψ>, ξo, ξi)> bfn;


struct γffn_ : public virtual γ
{
  γffn_(Stc &n_, ffn &&f_) : n(n_), f(new ffn(std::move(f_))) {}

  St name() const { return n; }

  Ξ &operator()(Ξ &x)
    { let q = Sp<ψ>(new ψ(x.t()));
      let [i, o] = x.xf(q);
      q->name(name())
        .def([i=i, o=o, f=f, q=q]() { (*f)(q, i, o); });
      return x; }

protected:
  St      n;
  Sp<ffn> f;
};


struct γbfn_ : public virtual γ
{
  γbfn_(Stc &n_, bfn &&f_) : n(n_), f(new bfn(std::move(f_))) {}

  St name() const { return n; }

  Ξ &operator()(Ξ &x)
    { let q = Sp<ψ>(new ψ(x.t()));
      let [i, o] = x.xf(q);
      q->name(name())
        .def([i=i, o=o, f=f, q=q]() { (*f)(q, o, i); });
      return x; }

protected:
  St      n;
  Sp<bfn> f;
};


Sp<γ> γffn(St n, ffn &&f) { return Sp<γ>(new γffn_(n, std::move(f))); }
Sp<γ> γbfn(St n, bfn &&f) { return Sp<γ>(new γbfn_(n, std::move(f))); }


}

#include "../end.hh"
