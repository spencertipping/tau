#ifndef tau_flux_h
#define tau_flux_h


// We don't depend on 32-bit boost::context, as that would create
// extra dev dependencies
#if defined(__EMSCRIPTEN__) || defined(__LP64__)
# define tau_defines_flux 1

# include "flux/init.hh"
# include "flux/lambda.hh"
# include "flux/Lambda.hh"
# include "flux/zeta.hh"
#else
# define tau_defines_flux 0
#endif


#endif
