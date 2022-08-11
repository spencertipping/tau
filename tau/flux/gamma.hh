#ifndef tau_flux_γ_h
#define tau_flux_γ_h


#include "../types.hh"
#include "../utf9.hh"
#include "types.hh"
#include "Gamma.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct γ
{
  Γ &g;
  γi i;


  γ(Γ &g_, γi i_) : g(g_), i(i_) {}



};


}

#include "../module/end.hh"


#endif
