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


struct Γ
{
  Λ l;
  Ψ q{l};
  Θ h{l};
  Φ f{l};



};


}

#include "../module/end.hh"


#endif
