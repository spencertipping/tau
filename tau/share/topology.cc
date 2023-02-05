#include "topology.hh"
#include "../begin.hh"

namespace τ
{


typedef F<void(Sp<ψ>, ξi, ξo)> ffn;
typedef F<void(Sp<ψ>, ξo, ξi)> bfn;

typedef F<void(ψ&)> fin;


struct γffn_ : public virtual γ
{
  γffn_(Stc &n_, ffn &&f_, fin &&xf_)
    : n(n_), f(new ffn(std::move(f_))), xf(new fin(std::move(xf_))) {}

  St name() const { return n; }

  Ξ &operator()(Ξ &x)
    { let q = x.q(name());
      let [i, o] = x.xf(q);
      q->xf([xf=xf](ψ &q) { (*xf)(q); });
      q->def([i=i, o=o, f=f, q=q]() mutable
        { (*f)(q, i, o);
          o.close();
          i.close(); });
      return x; }

protected:
  St      n;
  Sp<ffn> f;
  Sp<fin> xf;
};


struct γbfn_ : public virtual γ
{
  γbfn_(Stc &n_, bfn &&f_, fin &&xf_)
    : n(n_), f(new bfn(std::move(f_))), xf(new fin(std::move(xf_))) {}

  St name() const { return n; }

  Ξ &operator()(Ξ &x)
    { let q = x.q(name());
      let [o, i] = x.xb(q);
      q->xf([xf=xf](ψ &q) { (*xf)(q); });
      q->def([i=i, o=o, f=f, q=q]() mutable
        { (*f)(q, o, i);
          o.close();
          i.close(); });
      return x; }

protected:
  St      n;
  Sp<bfn> f;
  Sp<fin> xf;
};


struct γcap_ : public virtual γ
{
  γcap_(Stc &n_, bfn &&f_) : n(n_), f(new bfn(std::move(f_))) {}

  St name() const { return "]" + n; }

  Ξ &operator()(Ξ &x)
    { let q = x.q(name());
      q->def([q, f=f, o=x.p().fo(), i=x.p().fi()]() mutable
        { (*f)(q, o, i);
          o.close();
          i.close(); });
      return x; }

protected:
  St      n;
  Sp<bfn> f;
};


struct γcat_ : public virtual γ
{
  γcat_(V<Sp<γ>> &&gs_) : gs(std::move(gs_)) {}

  St name() const
    { Ss r;
      r << "[";
      for (let &x : gs) r << "+" << x->name();
      r << "]";
      return r.str(); }

  Ξ &operator()(Ξ &x)
    { V<ξi> rs;
      for (let &g : gs)
      { auto y = x.empty();
        rs.push_back((*g)(y).p().fi()); }
      let q = x.q(name());
      let [i, o] = x.xf(q);
      q->def([rs=std::move(rs), i=i, o=o]() mutable
        { for (let x : i) if (!(o << x)) goto done;
          for (auto &r : rs)
          { for (let x : r) if (!(o << x)) goto done;
            r.close(); }
        done:
          o.close();
          i.close(); });
      return x; }

protected:
  V<Sp<γ>> gs;
};


Sp<γ> γffn(St n, ffn &&f, fin &&xf) { return Sp<γ>(new γffn_(n, std::move(f), std::move(xf))); }
Sp<γ> γbfn(St n, bfn &&f, fin &&xf) { return Sp<γ>(new γbfn_(n, std::move(f), std::move(xf))); }

Sp<γ> γcat(V<Sp<γ>> &&gs) { return Sp<γ>(new γcat_(std::move(gs))); }


Sp<γ> γcap(St n, bfn &&f) { return Sp<γ>(new γcap_(n, std::move(f))); }


Sp<γ> γeach(F<void(η0i)> &&f, bool tap)
{
  return γffn("e", [tap, f=std::move(f)](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i)
      { f(η0i(x));
        if (tap && !(o << x)) break; }});
}


Sp<γ> γmap(F<η0o(η0i)> &&f)
{
  return γffn("m", [f=std::move(f)](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i) if (!(o << f(η0i(x)))) break; });
}


Sp<γ> γτmap(F<η0o(η0i)> &&f)
{
  return γffn("M", [f=std::move(f)](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i) if (!(o << f(η0i(x))) || !(o << η0o{η0sig::τ})) break; });
}


}

#include "../end.hh"
