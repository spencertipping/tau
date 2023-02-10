#include "routing.hh"

#include "../begin.hh"

namespace τ
{


struct γrfn_ : public virtual γ_
{
  γrfn_(Stc &n_, F<void(Ξ&)> &&f_)
    : n(n_), f(new F<void(Ξ&)>(std::move(f_))) {}

  St name() const { return n; }
  void operator()(Ξ &x) const { (*f)(x); }

protected:
  St              n;
  Sp<F<void(Ξ&)>> f;
};


γ γrfn(St n, F<void(Ξ&)> &&f)
{
  return new γrfn_(n, std::move(f));
}

γ γswap(St k)
{
  return γrfn("//" + k, [k](Ξ &x)
    { x(k);  // create unless ∃
      x.n()[k] = x.p(x.n().at(k)); });
}

γ γsub(St k, γ g)
{
  return γrfn("[" + g.name() + "]@" + k,
              [k, g](Ξ &x) { g(x[k]); });
}


struct γswap_ : public virtual γ_
{
  γswap_() {}
  St name() const { return "%"; }
  void operator()(Ξ &x) const { x.p().swap(); }
};


γ γb(γ g)
{
  return γξswap() | g | γξswap();
}


γ γξswap()
{
  return γrfn("%", [](Ξ &x) { x.p().swap(); });
}


}

#include "../end.hh"
