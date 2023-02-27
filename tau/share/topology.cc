#include "topology.hh"
#include "../begin.hh"

namespace τ
{


typedef F<void(Sp<ψ>, ξi, ξo)> ffn;
typedef F<void(Sp<ψ>, ξo, ξi)> bfn;

typedef F<void(ψ&)> fin;


struct Γffn_ : public virtual Γ_
{
  Γffn_(Stc &n_, ffn &&f_, fin &&xf_)
    : n(n_), f(new ffn(std::move(f_))), xf(new fin(std::move(xf_))) {}

  St name() const { return n; }

  void operator()(Ξ &x) const
    { let q = x.q(name());
      let [i, o] = x.xf(q);
      q->xf([xf=xf](ψ &q) { (*xf)(q); });
      q->def([i=i, o=o, f=f, q=q]() mutable
        { (*f)(q, i, o);
          o.close();
          i.close(); }); }

protected:
  St      n;
  Sp<ffn> f;
  Sp<fin> xf;
};


struct Γbfn_ : public virtual Γ_
{
  Γbfn_(Stc &n_, bfn &&f_, fin &&xf_)
    : n(n_), f(new bfn(std::move(f_))), xf(new fin(std::move(xf_))) {}

  St name() const { return n; }

  void operator()(Ξ &x) const
    { let q = x.q(name());
      let [o, i] = x.xb(q);
      q->xf([xf=xf](ψ &q) { (*xf)(q); });
      q->def([i=i, o=o, f=f, q=q]() mutable
        { (*f)(q, o, i);
          o.close();
          i.close(); }); }

protected:
  St      n;
  Sp<bfn> f;
  Sp<fin> xf;
};


struct Γcap_ : public virtual Γ_
{
  Γcap_(Stc &n_, ffn &&f_) : n(n_), f(new ffn(std::move(f_))) {}

  St name() const { return "]" + n; }

  void operator()(Ξ &x) const
    { let q = x.q(name());
      q->def([q, f=f, o=x.p().ro(), i=x.p().ri()]() mutable
        { (*f)(q, i, o);
          o.close();
          i.close(); }); }

protected:
  St      n;
  Sp<ffn> f;
};


struct Γcat_ : public virtual Γ_
{
  Γcat_(V<Γ> &&gs_) : gs(std::move(gs_)) {}

  St name() const
    { Ss r;
      r << "[";
      for (let &x : gs) r << "+" << x.name();
      r << "]";
      return r.str(); }

  void operator()(Ξ &x) const
    { V<ξi> rs;
      for (let &g : gs)
      { auto y = x.empty();
        rs.push_back(g(y).p().ri()); }
      let q = x.q(name());
      let [i, o] = x.xf(q);
      q->def([rs=std::move(rs), i=i, o=o]() mutable
        { for (let x : i) if (!(o << x)) goto done;
          for (auto &r : rs)
          { for (let x : r) if (!(o << x)) goto done;
            r.close(); }
        done:
          o.close();
          i.close(); }); }

protected:
  V<Γ> gs;
};


Γ Γffn(St n, ffn &&f, fin &&xf) { return new Γffn_(n, std::move(f), std::move(xf)); }
Γ Γbfn(St n, bfn &&f, fin &&xf) { return new Γbfn_(n, std::move(f), std::move(xf)); }

Γ Γcat(V<Γ> &&gs) { return new Γcat_(std::move(gs)); }


Γ Γcap(St n, ffn &&f) { return new Γcap_(n, std::move(f)); }


Γ Γeach(F<void(η)> &&f, bool tap)
{
  return Γffn("e", [tap, f=std::move(f)](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i)
      { f(x);
        if (tap && !(o << x)) break; }});
}


Γ Γmap(F<η0o(η)> &&f)
{
  return Γffn("m", [f=std::move(f)](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i) if (!(o << f(x))) break; });
}


Γ Γτmap(F<η0o(η)> &&f)
{
  return Γffn("M", [f=std::move(f)](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i) if (!(o << f(x)) || !(o << η1o(η1sig::τ))) break; });
}


}

#include "../end.hh"
