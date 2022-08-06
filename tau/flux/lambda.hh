#ifndef tau_flux_λ_h
#define tau_flux_λ_h


#include "lambda-class.hh"

#if defined(__EMSCRIPTEN__)
# include "lambda-emscripten.hh"
#elif defined(__LP64__)
# include "lambda-boost.hh"
#else
# error λ not supported on this architecture (trying to load tau::flux)
#endif


#endif
