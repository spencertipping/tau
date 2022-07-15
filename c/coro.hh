#ifndef tau_coro_h
#define tau_coro_h


#ifdef __EMSCRIPTEN__
#include "coro/emscripten.hh"
#else
#include "coro/boost.hh"
#endif


#endif
