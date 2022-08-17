#ifndef tau_flux_types_h
#define tau_flux_types_h


#include <chrono>
#include <cmath>


#include "../arch.hh"
#include "../types.hh"
#include "../utf9.hh"
#include "../util/shd.hh"
#include "../util/hash.hh"


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
typedef i64     Λr;  // type of value returned to Λ
typedef F<Λr()> λf;

typedef u64     γsi;
typedef uN      ψi;  // Γ-local port identifier
typedef uN      φi;  // internal connection identifier
typedef uN      γi;


constexpr γsi γsi_str(char const *s, uN l)
{
  if (l >= 8) return sha(std::string_view(s, l)).xs[0];
  u64 r = 0;
  for (uN i = 0; i < l; ++i) r |= Sc<u64>(Sc<u8>(s[i])) << (7 - i) * 8;
  return r;
}


constexpr uNc ζc0 = 65536;


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
  λφc,   // waiting for φc on a ψ
  λφx,   // waiting for φx on a ψ
  λZ,    // done (zombie)
};


#if tau_debug_iostream
O &operator<<(O &s, λs t)
{
  switch (t)
  {
  case λR:  return s << "R";
  case λS:  return s << "S";
  case λI:  return s << "I";
  case λO:  return s << "O";
  case λW:  return s << "W";
  case λΘ:  return s << "Θ";
  case λφc: return s << "φc";
  case λφx: return s << "φx";
  case λZ:  return s << "Z";
  default:  return s << "BOGUS " << Su(t);
  }
}
#endif


}

#include "../module/end.hh"


#endif
