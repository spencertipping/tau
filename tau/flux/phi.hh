#ifndef tau_flux_φ_h
#define tau_flux_φ_h


#include "phi-class.hh"


#if defined(__EMSCRIPTEN__)
# include "phi-emscripten.hh"
#else
# include "phi-epoll.hh"
#endif


#endif
