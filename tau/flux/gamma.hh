#ifndef tau_flux_γ_h
#define tau_flux_γ_h


#include "../types.hh"
#include "../utf9.hh"
#include "../util/hash.hh"

#include "types.hh"
#include "Gamma.hh"


#include "../module/begin.hh"
#include <string_view>

namespace tau::flux
{


struct γ
{
  Γ          &g;
  γi   const  i;
  γsi         gsi {0};
  M<γsi, λi>  yλ;
  M<γsi, ψi>  yψ;
  Θp          t0;

  γ(Γ &g_, γi i_) : g(g_), i(i_), t0(now()) {}
  ~γ()
    { g.ye = false;
      V<γsi> ls(yλ.size()); for (let &[l, _] : yλ) ls.push_back(l);
      V<γsi> qs(yψ.size()); for (let &[q, _] : yψ) qs.push_back(q);
      for (let &l : ls) λx(l);
      for (let &q : qs) ψw(q, ω);
      g.ye = true; }

  γsi        gs()                { while (yλ.contains(gsi) || yψ.contains(gsi)) ++gsi; return gsi; }
  ΔΘ         Θi()          const { return now() - t0; }
  ΣΘΔ        γΘ()          const { ΣΘΔ r; for (let &[_, l] : yλ) r += g.λΘ(i, l);                   return r; }

  γ         &λc (γsi s, λf f)   { assert(!yλ.contains(s)); yλ[s] = g.λc(i, std::move(f));          return *this; }
  bool       λe (γsi s)   const {                                                                  return yλ.contains(s); }
  Λr         λw (γsi s)         { let r = g.λw(i, yλ.at(s)); yλ.erase(s);                          return r; }
  bool       λwi(γsi s)   const {                                                                  return λe(s) && g.λwi(i, yλ.at(s)); }
  γ         &λx (γsi s)         { if (λe(s)) g.λx(i, yλ.at(s)); yλ.erase(s);                       return *this; }
  ΣΘΔ const &λΘ (γsi s)         {                                                                  return g.λΘ(i, yλ.at(s)); }

  bool φe (γsi s)  const { return g.φe (i, yψ.at(s)); }
  bool ψφw(γsi s)  const { return g.ψφw(i, yψ.at(s)); }
  bool ψxw(γsi s)  const { return g.ψxw(i, yψ.at(s)); }

  bool ψri(γsi s)  const { return g.ψri(i, yψ.at(s)); }
  bool ψrw(γsi s)  const { return g.ψrw(i, yψ.at(s)); }
  u9   ψr (γsi s)  const { return g.ψr (i, yψ.at(s)); }
  γ   &ψm (γsi f, γsi t) { assert(!yψ.contains(t)); yψ[t] = g.ψm(i, yψ.at(f)); return *this; }

  bool ψwi(γsi s) const { return yψ.contains(s) && g.ψwi(i, yψ.at(s)); }
  bool ψww(γsi s) const { return yψ.contains(s) && g.ψww(i, yψ.at(s)); }

  bool ψw(γsi s, u9 &&v)
    { if      (v == α && !yψ.contains(s)) { yψ[s] = g.ψc(i);                                    return true; }
      else if (v == ω)                    { if (yψ.contains(s)) g.ψx(i, yψ.at(s)), yψ.erase(s); return true; }
      return yψ.contains(s) && φe(s) && g.ψw(i, yψ.at(s), std::move(v)); }

  bool ψw(γsi s, u9c &v)
    { if      (v == α && !yψ.contains(s)) { yψ[s] = g.ψc(i);                                    return true; }
      else if (v == ω)                    { if (yψ.contains(s)) g.ψx(i, yψ.at(s)), yψ.erase(s); return true; }
      return yψ.contains(s) && φe(s) && g.ψw(i, yψ.at(s), v); }

  γ &Θw(ΔΘ t) { return Θw(now() + t); }
  γ &Θw(Θp t) { g.Θw(i, t); return *this; }
};


inline Γ::~Γ()
{
  // NOTE: concurrent modification can happen if by deleting a γ we also
  // modify gs (which happens in ~γ)
  V<γ*> ps(gs.size());
  for (let &[_, p] : gs) ps.push_back(p);
  for (let p : ps) delete p;
}


inline γ &Γ::γc()
{
  let i = ιi(gi, gs);
  return *(gs[i] = new γ{*this, i});
}


inline Γ &Γ::γx(γi i)
{
  delete gs[i];
  gs.erase(i);
  return *this;
}


inline Γ &Γ::φc(γi a, γsi aq, γi b, γsi bq, uN ζc)
{
  return φc(*gs.at(a), aq, *gs.at(b), bq, ζc);
}


inline Γ &Γ::φc(γ const &a, γsi aq, γ const &b, γsi bq, uN ζc)
{
  q.φc(a.yψ.at(aq), b.yψ.at(bq), ζc);
  return *this;
}


inline Γ &Γ::φx(γi a, γsi aq, γi b, γsi bq)
{
  return φx(*gs.at(a), aq, *gs.at(b), bq);
}


inline Γ &Γ::φx(γ const &a, γsi aq, γ const &b, γsi bq)
{
  let ai = a.yψ.at(aq);
  let bi = b.yψ.at(bq);
  if (q.φe(ai)) q.φx(ai);
  if (q.φe(bi)) q.φx(bi);
  return *this;
}


#if tau_debug_iostream
O &operator<<(O &s, γ const &g)
{
  return s << "γ TODO";
}
#endif


}

#include "../module/end.hh"


#endif
