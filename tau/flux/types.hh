#ifndef tau_flux_types_h
#define tau_flux_types_h


#include <chrono>


#include "../types.hh"
#include "../util/shd.hh"


#include "../module/begin.hh"

namespace tau::flux
{


using namespace std::literals;
using namespace tau::util;

#if tau_debug_iostream
using tau::util::operator<<;
#endif


typedef uN      λi;  // λ identifier
typedef f64     λp;  // λ priority (lower = more important)
typedef int     Λr;  // type of value returned to Λ
typedef F<Λr()> λf;


enum λs  // lambda runnability state
{
  λR,    // runnable
  λS,    // stopped
  λI,    // blocked on read from ζ
  λO,    // blocked on write to ζ
  λΘ,    // waiting for a time
  λZ,    // done (zombie)
};


#if tau_debug_iostream
O &operator<<(O &s, λs const &t)
{
  switch (t)
  {
  case λR: return s << "R";
  case λS: return s << "S";
  case λI: return s << "I";
  case λO: return s << "O";
  case λΘ: return s << "Θ";
  case λZ: return s << "Z";
  default: return s << "BOGUS " << Su(t);
  }
}
#endif


}

#include "../module/end.hh"


#endif
