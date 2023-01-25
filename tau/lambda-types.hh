#ifndef τλtypes_h
#define τλtypes_h


#include "arch.hh"
#include "debug.hh"

#include "ctypes.hh"


#include "begin.hh"

namespace τ
{


typedef uN        λi;  // λ identifier
typedef uN        λt;  // λ tag (for debugging)
typedef F<void()> λf;  // λ driver function


enum class λs : u8  // lambda runnability state
{
  R,   // runnable
  S,   // stopped
  Y,   // explicitly yielded
  I,   // blocked on read from ζ
  O,   // blocked on write to ζ
  W,   // waiting for a λ
  Θ,   // waiting for a time
  ΦI,  // blocked on read from fd
  ΦO,  // blocked on write to fd
  φc,  // waiting for φc on a φ
  φx,  // waiting for φx on a φ
};


#if τdebug_iostream
O &operator<<(O &s, λs t)
{
  switch (t)
  {
  case λs::R:  return s << "R";
  case λs::S:  return s << "S";
  case λs::Y:  return s << "Y";
  case λs::I:  return s << "I";
  case λs::O:  return s << "O";
  case λs::W:  return s << "W";
  case λs::Θ:  return s << "Θ";
  case λs::ΦI: return s << "ΦI";
  case λs::ΦO: return s << "ΦO";
  case λs::φc: return s << "φc";
  case λs::φx: return s << "φx";
    TA(s, "illegal λs " << Sc<uN>(t));
  }
}
#endif


}

#include "end.hh"


#endif
