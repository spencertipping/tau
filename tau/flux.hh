#ifndef tau_flux_h
#define tau_flux_h


#if defined(__EMSCRIPTEN__) || defined(__LP64__)
#  include "flux/init.hh"
#  include "flux/coro.hh"
#  include "flux/pipe.hh"
#  include "flux/defs.hh"
#  include "flux/lambda-defs.hh"
#  include "flux/lambda.hh"
#else
#  warning compiling without flux support (no emscripten or LP64)
#endif


#endif
