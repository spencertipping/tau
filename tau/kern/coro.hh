#ifndef tau_kern_coro_h
#define tau_kern_coro_h


#include "coro-class.hh"

#ifdef __EMSCRIPTEN__
#  include "coro-emscripten.hh"
#else
#  include "coro-boost.hh"
#endif


#endif
