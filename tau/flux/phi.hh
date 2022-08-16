#ifndef tau_flux_φ_h
#define tau_flux_φ_h


#include "../arch.hh"
#include "phi-class.hh"


#if tau_arch == tau_arch_wasm64 || tau_arch == tau_arch_wasm32
# include "phi-emscripten.hh"
#elif tau_arch == tau_arch_linux64 || tau_arch == tau_arch_linux32
# include "phi-epoll.hh"
#else
# error unsupported architecture for φ
#endif


#endif
