#ifndef tau_flux_h
#define tau_flux_h


// We don't depend on 32-bit boost::context, as that would create
// extra dev dependencies
#if defined(__EMSCRIPTEN__) || defined(__LP64__)
#  define tau_defines_flux 1

#  include "flux/init.hh"
#  include "flux/coro.hh"
#  include "flux/pipe.hh"
#  include "flux/defs.hh"
#  include "flux/lambda-defs.hh"
#  include "flux/lambda.hh"
#else
#  define tau_defines_flux 0
#endif


#endif
