#ifndef τshd_h
#define τshd_h


#include <chrono>
#include <numeric>
#include <type_traits>


#include "debug.hh"
#include "types.hh"

#include "Sigma.hh"


#include "begin.hh"

namespace τ
{

using namespace std::literals;


typedef std::chrono::steady_clock       Θc;
typedef std::chrono::nanoseconds        ΔΘ;
typedef std::chrono::time_point<Θc, ΔΘ> Θp;


// Definition of time quantum
ΔΘ const constexpr Θq = 1ns;
static_assert(Θq.count() == 1);
static_assert(std::is_integral<ΔΘ::rep>::value);


inline           Θp now()     { return Θc::now(); }
inline constexpr Θp never()   { return Θp{0ns}; }
inline constexpr Θp forever() { return Θp{Nl<typename Θp::duration::rep>::max() * Θq}; }


template<class H>
struct ΣΘΔ_
{
  bool Θr {false};
  Θp   Θl {0ns};
  ΔΘ   ΣΔ {0};
  u64  n  {0};
  H    h;

  ΔΘ       p         (f64 p)  const {                      return Θq * (1ull << h.icdf(p) + 1); }
  ΔΘ       Σ         ()       const {                      return ΣΔ + (Θr ? now() - Θl : 0ns); }
  ΔΘ       μ         ()       const { if (!n) return 0ns;  return ΣΔ / n; }

  ΣΘΔ_<H> &start     ()                 { A(!Θr, "start Θr"); Θl = now(); Θr = true;          return *this; }
  ΔΘ       stop      ()                 { let s = cancel(); *this << s;                       return s; }
  ΔΘ       cancel    ()                 { A(Θr, "cancel !Θr"); Θr = false;                    return now() - Θl; }
  ΣΘΔ_<H> &operator+=(ΣΘΔ_<H> const &s) { A(!s.Θr, "+= Θr"); ΣΔ += s.ΣΔ; n += s.n; h += s.h;  return *this; }
  ΣΘΔ_<H> &operator<<(ΔΘ s)             { ΣΔ += s; h << s.count(); ++n;                       return *this; }
};


typedef ΣΘΔ_<Σι> ΣΘΔ;
typedef ΣΘΔ_<Σ0>  ΘΔ;


#if τdebug_iostream
O &operator<<(O &s, ΔΘ const &t)
{
  let a = abs(t);
  if      (a <= 100us) return s << t.count() << "ns";
  else if (a <= 100ms) return s << t / 1us   << "μs";
  else if (a <= 100s)  return s << t / 1ms   << "ms";
  else if (a <= 1h)    return s << t / 1s    << "s";
  else if (a <= 10h)   return s << t / 1min  << "m";
  else                 return s << t / 1h    << "h";
  return s;
}

O &operator<<(O &s, Θp const &p)
{
  return s << "t+" << p - now();
}

O &operator<<(O &s, ΣΘΔ const &w)
{
  return s << "ΣΘΔ[" << (w.Θr ? "R" : "s")
           << " n="  << w.n
           << " Σ="  << w.Σ()
           << " μ="  << w.μ()
           << " 50=" << w.p(0.5)
           << " 90=" << w.p(0.9)
           << " 99=" << w.p(0.99) << "]";
}

O &operator<<(O &s, ΘΔ const &w)
{
  return s << "ΘΔ[" << (w.Θr ? "R" : "s")
           << " n=" << w.n
           << " Σ=" << w.Σ()
           << " μ=" << w.μ() << "]";
}
#endif


}

#include "end.hh"


#endif
