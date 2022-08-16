#ifndef tau_flux_γ_h
#define tau_flux_γ_h


#include "../types.hh"
#include "../utf9.hh"
#include "../util/hash.hh"

#include "types.hh"
#include "Gamma.hh"


#include "../module/begin.hh"

namespace tau::flux
{


using tau::util::sha256;


struct γ
{
  Γ             &g;
  γi      const  i;
  sha256         gsi{0, 0, 0, 0};
  M<sha256, λi>  yλ;
  M<sha256, ψi>  yψ;
  Θp             t0;

  γ(Γ &g_, γi i_) : g(g_), i(i_), t0(now()) {}

  u9         gs()                { return u9{++gsi}; }
  ΔΘ         Θi()          const { return now() - t0; }
  ΣΘΔ        γΘ()          const { ΣΘΔ r; for (let &[_, l] : yλ) r += g.λΘ(i, l);                   return r; }

  γ         &λc (u9c &s, λf f)   { assert(!yλ.contains(s)); yλ[s] = g.λc(i, std::move(f));          return *this; }
  bool       λe (u9c &s)   const {                                                                  return yλ.contains(s); }
  Λr         λw (u9c &s)         { let r = g.λw(i, yλ.at(s)); yλ.erase(s);                          return r; }
  bool       λwi(u9c &s)   const {                                                                  return g.λwi(i, yλ.at(s)); }
  γ         &λx (u9c &s)         { g.λx(i, yλ.at(s));         yλ.erase(s);                          return *this; }
  ΣΘΔ const &λΘ (u9c &s)         {                                                                  return g.λΘ(i, yλ.at(s)); }

  bool φe (u9c &s) const   { return g.φe (i, yψ.at(s)); }
  bool ψφw(u9c &s) const   { return g.ψφw(i, yψ.at(s)); }
  bool ψxw(u9c &s) const   { return g.ψxw(i, yψ.at(s)); }

  bool ψri(u9c &s) const   { return g.ψri(i, yψ.at(s)); }
  bool ψrw(u9c &s) const   { return g.ψrw(i, yψ.at(s)); }
  u9   ψr (u9c &s) const   { return g.ψr (i, yψ.at(s)); }
  γ   &ψm (u9c &f, u9c &t) { assert(!yψ.contains(t)); yψ[t] = g.ψm(i, yψ.at(f)); return *this; }

  bool ψwi(u9c &s) const { return g.ψwi(i, yψ.at(s)); }
  bool ψww(u9c &s) const { return g.ψww(i, yψ.at(s)); }

  bool ψw(u9c &s, u9 &&v)
    { if      (v == α) { assert(!yψ.contains(s)); yψ[s] = g.ψc(i); return true; }
      else if (v == ω) { g.ψx(i, yψ.at(s)); yψ.erase(s);           return true; }
      return g.ψw(i, yψ.at(s), std::move(v)); }

  bool ψw(u9c &s, u9c &v)
    { if      (v == α) { assert(!yψ.contains(s)); yψ[s] = g.ψc(i); return true; }
      else if (v == ω) { g.ψx(i, yψ.at(s)); yψ.erase(s);           return true; }
      return g.ψw(i, yψ.at(s), v); }

  γ &Θw(ΔΘ t) { return Θw(now() + t); }
  γ &Θw(Θp t) { g.Θw(i, t); return *this; }
};


inline Γ::~Γ()
{
  for (let &[_, p] : gs) delete p;
}


inline γ &Γ::γc()
{
  return *(gs[ιi(gi, gs)] = new γ{*this, gi});
}


inline Γ &Γ::γx(γi i)
{
  let &g = *gs[i];
  ye = false;
  for (let &[_, l] : g.yλ) λx(i, l);
  for (let &[_, q] : g.yψ) ψx(i, q);
  ye = true;
  delete gs[i];
  gs.erase(i);
  return *this;
}


inline Γ &Γ::φc(γi a, u9c &aq, γi b, u9c &bq, uN ζc)
{
  return φc(*gs.at(a), aq, *gs.at(b), bq, ζc);
}


inline Γ &Γ::φc(γ const &a, u9c &aq, γ const &b, u9c &bq, uN ζc)
{
  q.φc(a.yψ.at(aq), b.yψ.at(bq), ζc);
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
