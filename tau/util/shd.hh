#ifndef tau_util_shd_h
#define tau_util_shd_h


#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>
#include <type_traits>


#include "../types.hh"

#include "pi.hh"


#include "../module/begin.hh"

namespace tau::util
{

using namespace std::literals;


typedef std::chrono::steady_clock       Θc;
typedef std::chrono::nanoseconds        ΔΘ;
typedef std::chrono::time_point<Θc, ΔΘ> Θp;

ΔΘ const constexpr Θq = 1ns;


inline           Θp now()     { return Θc::now(); }
inline constexpr Θp never()   { return Θp{0ns}; }
inline constexpr Θp forever() { return Θp{Nl<typename Θp::duration::rep>::max() * Θq}; }


// Definition of time quantum
static_assert(Θq.count() == 1);
static_assert(std::is_integral<ΔΘ::rep>::value);


struct ΣΘΔ
{
  bool Θr {false};
  Θp   Θl {0ns};
  ΔΘ   ΣΔ {0};
  u64  n  {0};
  πι   h;

  ΔΘ   p         (f64 p)  const { return Θq * (1ull << h.icdf(p) + 1); }
  ΔΘ   Σ         ()       const { return ΣΔ + (Θr ? now() - Θl : 0ns); }
  ΔΘ   σ         ()       const { if (!n) return 0ns; return ΣΔ / n; }

  ΣΘΔ &start     ()             { assert(!Θr); Θl = now(); Θr = true; return *this; }
  ΔΘ   stop      ()             { let s = cancel(); *this << s; return s; }
  ΔΘ   cancel    ()             { assert(Θr); Θr = false; return now() - Θl; }
  ΣΘΔ &operator+=(ΣΘΔ const &s) { assert(!s.Θr); ΣΔ += s.ΣΔ; n += s.n; h += s.h; return *this; }
  ΣΘΔ &operator<<(ΔΘ s)         { ΣΔ += s; h << s.count(); ++n; return *this; }
};


O &operator<<(O &s, ΔΘ const &t)
{
  if      (t <= 100us) return s << t.count() << "ns";
  else if (t <= 100ms) return s << t / 1us   << "μs";
  else if (t <= 100s)  return s << t / 1ms   << "ms";
  else if (t <= 1h)    return s << t / 1s    << "s";
  else if (t <= 10h)   return s << t / 1min  << "m";
  else                 return s << t / 1h    << "h";
  return s;
}


O &operator<<(O &s, ΣΘΔ const &w)
{
  return s << "ΣΘΔ[" << (w.Θr ? "R" : "s")
           << " n=" << w.n
           << " Σ=" << w.Σ()
           << " p50=" << w.p(0.5)
           << " p90=" << w.p(0.9)
           << " p99=" << w.p(0.99) << "]";
}


}

#include "../module/end.hh"


#endif
