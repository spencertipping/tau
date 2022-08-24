#ifndef tau_flux_h
#define tau_flux_h


#include "arch.hh"

#define tau_defines_flux                        \
  (   tau_arch == tau_arch_linux64              \
   || tau_arch == tau_arch_wasm32               \
   || tau_arch == tau_arch_wasm64)


#if tau_defines_flux
# include "flux/lambda.hh"
# include "flux/Lambda.hh"
# include "flux/zetab.hh"
# include "flux/zetabits.hh"
# include "flux/zeta.hh"
#endif


#endif
