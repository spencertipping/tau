#include "routing.hh"

#include "../begin.hh"

namespace τ
{


struct Γrfn_ : public virtual Γ_
{
  Γrfn_(Stc &n_, F<void(Ξ&)> &&f_)
    : n(n_), f(new F<void(Ξ&)>(std::move(f_))) {}

  St name() const { return n; }
  void operator()(Ξ &x) const { (*f)(x); }

protected:
  St              n;
  Sp<F<void(Ξ&)>> f;
};


Γ Γrfn(St n, F<void(Ξ&)> &&f)
{
  return new Γrfn_(n, std::move(f));
}

Γ Γswap(St k)
{
  return Γrfn("//" + k, [k](Ξ &x)
    { x(k);  // create unless ∃
      x.n()[k] = x.p(x.n().at(k)); });
}

Γ Γsub(St k, Γ g)
{
  return Γrfn("[" + g.name() + "]@" + k,
              [k, g](Ξ &x) { g(x[k]); });
}


}

#include "../end.hh"
