#ifndef tau_fabric_monitor_h
#define tau_fabric_monitor_h


#include <cassert>
#include <chrono>
#include <cstdint>
#include <unordered_map>


#include "../coro.hh"

#include "stopwatch.hh"


#include "../begin.hh"


namespace tau::fabric
{

namespace tc = tau::coro;


struct cell_monitor
{
  template<class T>
  using co = tc::coro<T, cell_monitor>;

  stopwatch s;

  template<class T> void init    (co<T> &c) {}
  template<class T> void enter   (co<T> &c) { s.start(); }
  template<class T> void exit    (co<T> &c) { s.stop();  }
  template<class T> void finalize(co<T> &c) {}
};


}

#include "../end.hh"


#endif
