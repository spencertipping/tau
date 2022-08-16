#ifndef tau_flux_λ_h
#define tau_flux_λ_h


#include "../arch.hh"
#include "lambda-class.hh"

#if tau_arch == tau_arch_wasm32 || tau_arch == tau_arch_wasm64
# include "lambda-emscripten.hh"
#elif tau_arch == tau_arch_linux64
# include "lambda-boost.hh"
#else
# error λ not supported on this architecture (trying to load tau::flux)
#endif


#endif
