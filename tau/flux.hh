#ifndef tau_flux_h
#define tau_flux_h


// We don't depend on 32-bit boost::context, as that would create
// extra dev dependencies
#if defined(__EMSCRIPTEN__) || defined(__LP64__)
# define tau_defines_flux 1

# include "flux/types.hh"
# include "flux/lambda.hh"
# include "flux/Lambda.hh"
# include "flux/theta.hh"
# include "flux/u9z.hh"
# include "flux/psi.hh"
# include "flux/phi.hh"
# include "flux/zeta.hh"
# include "flux/Gamma.hh"
#else
# define tau_defines_flux 0
#endif


#endif
