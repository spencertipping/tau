#ifndef tau_flux_types_h
#define tau_flux_types_h


#include <chrono>


#include "../types.hh"
#include "../utf9.hh"
#include "../util/pi.hh"
#include "../util/shd.hh"


#include "../module/begin.hh"

namespace tau::flux
{


using namespace std::literals;
using namespace tau::util;

using tau::util::operator<<;


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


}

#include "../module/end.hh"


#endif
