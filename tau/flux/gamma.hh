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

  γ(Γ &g_, γi i_) : g(g_), i(i_) {}

  γ         &λc(u9 const &s, λf &&f) { assert(!yλ.contains(s)); yλ[s] = g.λc(i, std::move(f)); return *this; }
  bool       λe(u9 const &s)   const {                                                         return yλ.contains(s); }
  Λr         λw(u9 const &s)         { let r = g.λw(i, yλ.at(s)); yλ.erase(s);                 return r; }
  γ         &λx(u9 const &s)         { g.λx(i, yλ.at(s));         yλ.erase(s);                 return *this; }
  ΣΘΔ const &λΘ(u9 const &s)         {                                                         return g.λΘ(i, yλ.at(s)); }

  // TODO: ψ functions
};


inline γ &Γ::γc() { return *(gs[ιi(gi, gs)] = new γ{*this, gi}); }


#if tau_debug_iostream
O &operator<<(O &s, γ const &g)
{
  return s << "γ TODO";
}
#endif


}

#include "../module/end.hh"


#endif
