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


inline Θp now()     { return Θc::now(); }
ic     Θp never()   { return Θp{0ns}; }
ic     Θp forever() { return Θp{Nl<typename Θp::duration::rep>::max() * Θq}; }


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


O &operator<<(O &s, ΔΘ const &t);
O &operator<<(O &s, Θp const &p);
O &operator<<(O &s, ΘP const &p);
O &operator<<(O &s, ΣΘΔ const &w);
O &operator<<(O &s, ΘΔ const &w);


}

#include "end.hh"


#endif
