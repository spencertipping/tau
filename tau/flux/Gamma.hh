#ifndef tau_flux_Γ_h
#define tau_flux_Γ_h


#include "../types.hh"
#include "Lambda.hh"
#include "theta.hh"
#include "phi.hh"
#include "psi.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct γ;
struct Γ
{
  Λ  l;
  Ψ  q {l};
  Θ  h {l};
  Φ  f {l};
  γi gi{0};

  M<λi, γi> lg;
  M<ψi, γi> qg;
  M<γi, γ*> gs;

  γ &γc();
  Γ &γx(γi i);

  // TODO
  λi         λc(γi, λf &&f) {}
  Λr         λw(γi, λi)     {}
  Γ         &λx(γi, λi)     {}
  ΣΘΔ const &λΘ(γi, λi)     {}
};


#if tau_debug_iostream
O &operator<<(O &s, Γ const &g)
{
  return s << "Γ TODO";
}
#endif


}

#include "../module/end.hh"


#endif
