#ifndef tau_axis_channel_h
#define tau_axis_channel_h


#include <cstdint>
#include <queue>

#include "../coro.hh"

#include "../begin.hh"


namespace tau::axis
{

namespace tc = tau::coro;


template<class T>
struct channel
{
  uint16_t      istate;
  uint16_t      ostate;
  uint32_t      capacity;
  uint64_t      n;
  std::queue<T> xs;

  channel(uint32_t capacity_)
    : capacity(capacity_),
      istate  (0),
      ostate  (0),
      n       (0)
    {}


  // TODO: collect timing statistics
  channel &operator<<(T &x)
    { while (xs.size() >= capacity) tc::yield();
      xs.push(x);
      return *this; }
};


}


#include "../end.hh"

#endif
