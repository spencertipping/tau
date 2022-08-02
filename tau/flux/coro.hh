#ifndef tau_flux_coro_h
#define tau_flux_coro_h


#include "init.hh"
#include "coro-class.hh"

#ifdef __EMSCRIPTEN__
#  include "coro-emscripten.hh"
#else
#  include "coro-boost.hh"
#endif


#endif
