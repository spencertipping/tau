#include "topology.hh"
#include "../begin.hh"

namespace τ
{


typedef F<void(Sp<ψ>, ξi, ξo)> ffn;
typedef F<void(Sp<ψ>, ξo, ξi)> bfn;

typedef F<void(ψ&)> fin;


struct γffn_ : public virtual γ_
{
  γffn_(Stc &n_, ffn &&f_, fin &&xf_)
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


struct γbfn_ : public virtual γ_
{
  γbfn_(Stc &n_, bfn &&f_, fin &&xf_)
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


struct γcap_ : public virtual γ_
{
  γcap_(Stc &n_, bfn &&f_) : n(n_), f(new bfn(std::move(f_))) {}

  St name() const { return "]" + n; }

  void operator()(Ξ &x) const
    { let q = x.q(name());
      q->def([q, f=f, o=x.p().fo(), i=x.p().fi()]() mutable
        { (*f)(q, o, i);
          o.close();
          i.close(); }); }

protected:
  St      n;
  Sp<bfn> f;
};


struct γcat_ : public virtual γ_
{
  γcat_(V<γ> &&gs_) : gs(std::move(gs_)) {}

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
        rs.push_back(g(y).p().fi()); }
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
  V<γ> gs;
};


γ γffn(St n, ffn &&f, fin &&xf) { return new γffn_(n, std::move(f), std::move(xf)); }
γ γbfn(St n, bfn &&f, fin &&xf) { return new γbfn_(n, std::move(f), std::move(xf)); }

γ γcat(V<γ> &&gs) { return new γcat_(std::move(gs)); }


γ γcap(St n, bfn &&f) { return new γcap_(n, std::move(f)); }


γ γeach(F<void(η)> &&f, bool tap)
{
  return γffn("e", [tap, f=std::move(f)](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i)
      { f(x);
        if (tap && !(o << x)) break; }});
}


γ γmap(F<η0o(η)> &&f)
{
  return γffn("m", [f=std::move(f)](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i) if (!(o << f(x))) break; });
}


γ γτmap(F<η0o(η)> &&f)
{
  return γffn("M", [f=std::move(f)](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i) if (!(o << f(x)) || !(o << η1o(η1sig::τ))) break; });
}


}

#include "../end.hh"
