#include "topology.hh"
#include "../begin.hh"

namespace τ
{


struct Γ2_ : public virtual Γ_
{
  Γ2_(Stc &n_, Γffn_ &&f_, Γ2fn_ &&f2_, Γfin_ &&xf_)
    : n (n_),
      f (new Γffn_(std::move(f_))),
      f2(new Γ2fn_(std::move(f2_))),
      xf(new Γfin_(std::move(xf_))) {}

  St name() const { return n; }

  void operator()(Ξ &x) const
    { let q = x.q(name());
      let [i, o] = (*f2)(q, x);
      q->xf([xf=xf](ψ &q) { (*xf)(q); });
      q->def([i=i, o=o, f=f, q=q]() mutable
        { (*f)(q, i, o);
          o.close();
          i.close(); }); }

protected:
  St        n;
  Sp<Γffn_> f;
  Sp<Γ2fn_> f2;
  Sp<Γfin_> xf;
};


Γ2fn_ Γ2f()  { return [](Sp<ψ> q, Ξ &x) { return x.xf(q); }; }
Γ2fn_ Γ2b()  { return [](Sp<ψ> q, Ξ &x) { return flip(x.xb(q)); }; }
Γ2fn_ Γ2rc() { return [](Sp<ψ> q, Ξ &x) { return mp(x.p().f(), x.p().b()); }; }


Γ Γ2(St n, Γffn_ &&f, Γ2fn_ &&f2, Γfin_ &&xf)
{
  return Γ(new Γ2_(n, std::move(f), std::move(f2), std::move(xf)));
}


}

#include "../end.hh"
