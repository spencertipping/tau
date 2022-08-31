#ifndef tau_flux_λ_h
#define tau_flux_λ_h


#include "arch.hh"
#include "lambda-class.hh"

#if tau_platform == tau_platform_wasm
# include "lambda-emscripten.hh"
#elif tau_arch == tau_arch_linux64
# include "lambda-boost.hh"
#else
# error λ not supported on this architecture
#endif


#ifdef tau_assume_emscripten
# error tau_assume_emscripten is not intended for production
#endif


#endif
