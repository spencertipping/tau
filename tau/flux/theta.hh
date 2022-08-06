#ifndef tau_flux_theta_h
#define tau_flux_theta_h


#include "../types.hh"
#include "../util/shd.hh"

#include "types.hh"
#include "Lambda.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct Θ
{
  struct Θλ  { ΣΘΔ m; Θp d; };
  struct Θλp { Θ &h; bool operator()(λi a, λi b) const { return h.ls.at(a).d < h.ls.at(b).d; } };

  Λ           &l;
  Θλp          qp;
  M<λi, Θλ>    ls;
  PQ<λi, Θλp>  dq;

  Θ(Λ &l_) : l(l_), qp{*this}, dq(qp) {}

  Θ &c(λi i)       { ls.emplace(i, Θλ{ΣΘΔ{}, never()}); return *this; }
  Θ &x(λi i)       { ls.erase(i);                       return *this; }
  Θ &w(λi i, Θp t) { ls.at(i).d = t; dq.push(i);        return *this; }

  Θp δ()     const { return dq.empty() ? never() : ls.at(dq.top()).d; }

  Θ &r(λi i) { ls.at(i).m.start(); return *this; }

  bool y(λi i, f64 p = 0.9)
    { return true;  // FIXME
    }
};


}

#include "../module/end.hh"


#endif
