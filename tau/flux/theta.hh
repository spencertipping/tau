#ifndef tau_flux_θ_h
#define tau_flux_θ_h


#include "../types.hh"
#include "../util/shd.hh"

#include "types.hh"
#include "Lambda.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct Θ
{
  struct Θλ  { ΣΘΔ m; ΣΘΔ y; Θp d; Θλ() : d(never()) {} Θλ(Θp d_) : d(d_) {} };
  struct Θλp { Θ &h; bool operator()(λi a, λi b) const { return h.ls.at(a).d < h.ls.at(b).d; } };

  Λ           &l;
  Θλp          qp;
  M<λi, Θλ>    ls;
  PQ<λi, Θλp>  dq;

  Θ(Λ &l_) : l(l_), qp{*this}, dq(qp) {}

  Θ &c(λi i)       { ls[i] = Θλ(never());                                         return *this; }
  Θ &x(λi i)       { ls.erase(i);                                                 return *this; }
  Θ &w(λi i, Θp t) { auto &l = ls.at(i); assert(t <= l.d); l.d = t; dq.push(i);   return *this; }
  Θ &r(λi i)       { auto &l = ls.at(i); l.d = never(); l.m.start(); l.y.start(); return *this; }
  Θ &s(λi i)       { auto &l = ls.at(i);                l.m.stop();  l.y.stop();  return *this; }

  ΣΘΔ const &mi(λi i) const { return ls.at(i).m; }
  ΣΘΔ const &yi(λi i) const { return ls.at(i).y; }
  Θp          Δ()     const { return dq.empty() ? never() : ls.at(dq.top()).d; }

  // Probabilistic yield for upcoming deadline (returns true iff the specified
  // thread should yield now).
  //
  // NOTE: calling this function is intended to be an offer to yield, so the
  // λ's yield timer is updated.
  bool y(λi i, f64 p = 0.9)
    { auto &l = ls.at(i); l.y.stop(); l.y.start();
      return now() + l.y.p(p) <= Δ(); }
};


#if tau_debug_iostream
O &operator<<(O &s, Θ::Θλ const &l)
{
  s << "Θλ[";
  if (l.d != never()) s << " d=t+" << l.d - now();
  return s << l.m << " y=" << l.y << "]";
}

O &operator<<(O &s, Θ const &h)
{
  s << "Θ" << std::endl;
  for (let &[k, v] : h.ls)
    s << "  " << k << ": " << v << std::endl;
  return s;
}
#endif


}

#include "../module/end.hh"


#endif
