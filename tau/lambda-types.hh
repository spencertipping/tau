#ifndef τλtypes_h
#define τλtypes_h


#include "arch.hh"
#include "debug.hh"

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


#if τdebug_iostream
O &operator<<(O &s, λs t)
{
  switch (t)
  {
  case λs::R:  return s << "R";
  case λs::S:  return s << "S";
  case λs::Y:  return s << "Y";
  case λs::Θ:  return s << "Θ";
  case λs::ξR: return s << "ξR";
  case λs::ξW: return s << "ξW";
  case λs::τR: return s << "τR";
  case λs::τW: return s << "τW";
    TA(s, "illegal λs " << Sc<uN>(t));
  }
}
#endif


}

#include "end.hh"


#endif
