#include "routing.hh"

#include "../Gamma.hh"
#include "../begin.hh"

namespace τ
{


struct γrfn_ : public virtual γ
{
  γrfn_(Stc &n_, F<void(Ξ&)> &&f_)
    : n(n_), f(new F<void(Ξ&)>(std::move(f_))) {}

  St name() const { return n; }
  Ξ &operator()(Ξ &x) { (*f)(x); return x; }

protected:
  St              n;
  Sp<F<void(Ξ&)>> f;
};


Sp<γ> γrfn(St n, F<void(Ξ&)> &&f)
{
  return Sp<γ>(new γrfn_(n, std::move(f)));
}

Sp<γ> γswap(St k)
{
  return γrfn("//" + k, [k](Ξ &x)
    { x(k);  // create unless ∃
      x.n()[k] = x.p(x.n().at(k)); });
}

Sp<γ> γsub(St k, Sp<γ> g)
{
  return γrfn("[" + g->name() + "]@" + k,
              [k, g](Ξ &x) { (*g)(x[k]); });
}


struct γswap_ : public virtual γ
{
  γswap_() {}
  St name() const { return "%"; }
  Ξ &operator()(Ξ &x) { x.p().swap(); return x; }
};


Sp<γ> γb(Sp<γ> g)
{
  return γs(γξswap() | g | γξswap());
}


Sp<γ> γξswap()
{
  return γrfn("%", [](Ξ &x) { x.p().swap(); });
}


}

#include "../end.hh"
