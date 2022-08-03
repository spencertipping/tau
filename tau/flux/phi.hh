#ifndef tau_flux_φ_h
#define tau_flux_φ_h


#include <iostream>
#include <unordered_map>


#include "init.hh"
#include "defs.hh"
#include "lambda-defs.hh"


#include "../module/begin.hh"

namespace tau::flux
{


// λ/ψ IO block tracking: determine when λs become runnable in response
// to ψ events
struct Φ
{
  std::unordered_map<ψi, λi> r;
  std::unordered_map<ψi, λi> w;


};


}

#include "../module/end.hh"


#endif
