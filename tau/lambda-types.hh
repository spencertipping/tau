#ifndef τλtypes_h
#define τλtypes_h


#include "ctypes.hh"

#include "begin.hh"

namespace τ
{


typedef u64       λi;  // λ identifier
typedef F<void()> λf;  // λ driver function


enum class λs : u8  // lambda runnability state
{
  R,   // runnable
  S,   // stopped
  Y,   // explicitly yielded (for one quantum)
  Θ,   // waiting for a time
  ξR,  // blocked on read from ξ
  ξW,  // blocked on write to ξ
  τR,  // blocked on read from fd
  τW,  // blocked on write to fd
};


O &operator<<(O&, λs);


}

#include "end.hh"


#endif
