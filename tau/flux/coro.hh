#ifndef tau_flux_coro_h
#define tau_flux_coro_h


#include "init.hh"
#include "coro-class.hh"

#if defined(__EMSCRIPTEN__)
#  include "coro-emscripten.hh"
#elif defined(__LP64__)
#  include "coro-boost.hh"
#else
#  error λ not supported on this architecture (trying to load tau::flux)
#endif


#endif
