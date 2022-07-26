#ifndef tau_coro_h
#define tau_coro_h


#include "coro/class.hh"

#ifdef __EMSCRIPTEN__
#  include "coro/emscripten.hh"
#else
#  include "coro/boost.hh"
#endif


#include "module/begin.hh"

namespace tau
{


template<class T>
using co = tau::coro::coro<T>;


}

#include "module/end.hh"


#endif
