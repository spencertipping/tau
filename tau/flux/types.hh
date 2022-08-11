#ifndef tau_flux_types_h
#define tau_flux_types_h


#include <chrono>
#include <cmath>


#include "../types.hh"
#include "../utf9.hh"
#include "../util/shd.hh"


#include "../module/begin.hh"

namespace tau::flux
{


using namespace std::literals;
using namespace tau::util;

#if tau_debug_iostream
using tau::util::operator<<;
#endif


// NOTE: we can use native ints even if they're just int32; to do this,
// we need a custom increment function ιi() that detects collisions from
// reuse.

typedef uN      λi;  // λ identifier
typedef f64     λp;  // λ priority (lower = more important)
typedef int     Λr;  // type of value returned to Λ
typedef F<Λr()> λf;

typedef uN      ψi;  // Γ-local port identifier
typedef uN      φi;  // internal connection identifier
typedef uN      γi;


uNc constexpr ζc₀ = 65536;


template<class K, class V>
inline K ιi(K &c, M<K, V> const &m)
{
  while (m.contains(++c) || !c);
  return c;
}


enum λs  // lambda runnability state
{
  λR,    // runnable
  λS,    // stopped
  λI,    // blocked on read from ζ
  λO,    // blocked on write to ζ
  λW,    // waiting for a λ
  λΘ,    // waiting for a time
  λZ,    // done (zombie)
};


#if tau_debug_iostream
O &operator<<(O &s, λs t)
{
  switch (t)
  {
  case λR: return s << "R";
  case λS: return s << "S";
  case λI: return s << "I";
  case λO: return s << "O";
  case λW: return s << "W";
  case λΘ: return s << "Θ";
  case λZ: return s << "Z";
  default: return s << "BOGUS " << Su(t);
  }
}
#endif


}

#include "../module/end.hh"


#endif
