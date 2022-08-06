#ifndef tau_util_stopwatch_h
#define tau_util_stopwatch_h


#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>


#include "../types.hh"

#include "log-histogram.hh"


#include "../module/begin.hh"

namespace tau::util
{

using namespace std::literals;


typedef std::chrono::nanoseconds ΔΘ;


template<class clk = std::chrono::steady_clock>
struct ΣΘΔ_
{
  typedef std::chrono::time_point<clk, ΔΘ> Θp;
  typedef log_histogram<u64, u64, 64>      lh;

  bool Θr {false};
  Θp   Θl {0ns};
  ΔΘ   ΣΔ {0};
  u64  n  {0};
  lh   h;


  ΔΘ p(f64 p) const { return 1ns * (1ull << h.icdf(p) + 1); }
  ΔΘ Σ()      const { return ΣΔ + (Θr ? now() - Θl : 0ns); }

  ΣΘΔ_ &operator<<(ΔΘ s)
    { ΣΔ += s;
      h << s.count();
      ++n;
      return *this; }

  ΔΘ σ() const
    { if (!n) return 0ns;
      return ΣΔ / n; }

  ΣΘΔ_ &start()
    { assert(!Θr);
      Θl = now();
      Θr = true;
      return *this; }

  ΔΘ stop()
    { assert(Θr);
      let s = now() - Θl;
      Θr = false;
      *this << s;
      return s; }


  ΣΘΔ_ &operator+=(ΣΘΔ_ const &s)
    { assert(!s.Θr);
      ΣΔ += s.ΣΔ;
      n      += s.n;
      h        += s.h;
      return *this; }


  static           Θp now()     { return clk::now(); }
  static constexpr Θp never()   { return Θp{0ns}; }
  static constexpr Θp forever() { return Θp{Nl<typename Θp::duration::rep>::max() * 1ns}; }
};


typedef ΣΘΔ_<> ΣΘΔ;


std::ostream &operator<<(std::ostream &s, ΔΘ const &t)
{
  if      (t <= 100us) return s << t.count() << "ns";
  else if (t <= 100ms) return s << t / 1us   << "μs";
  else if (t <= 100s)  return s << t / 1ms   << "ms";
  else if (t <= 1h)    return s << t / 1s    << "s";
  else if (t <= 10h)   return s << t / 1min  << "m";
  else                 return s << t / 1h    << "h";
  return s;
}


template<class T>
std::ostream &operator<<(std::ostream &s, ΣΘΔ_<T> const &w)
{
  return s << "sw[" << (w.Θr ? "R" : "s")
           << " n=" << w.n
           << " Σ=" << w.Σ()
           << " p50=" << w.p(0.5)
           << " p90=" << w.p(0.9)
           << " p99=" << w.p(0.99) << "]";
}


}

#include "../module/end.hh"


#endif
