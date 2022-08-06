#ifndef tau_flux_Λ_h
#define tau_flux_Λ_h


#include <cmath>


#include "../types.hh"
#include "init.hh"
#include "lambda.hh"


#include "../module/begin.hh"

namespace tau::flux
{


enum λs  // lambda runnability state (informative, not canonical)
{
  λR,    // runnable
  λS,    // stopped
  λI,    // blocked on read from ζ
  λO,    // blocked on write to ζ
  λΘ,    // waiting for a time
  λZ,    // done (zombie)
};


typedef uN      λi;  // λ identifier
typedef f64     λp;  // λ priority (lower = more important)
typedef int     Λr;  // type of value returned to Λ
typedef F<Λr()> λf;


// NOTE: managed λs should yield out with tau::flux::λy()
struct Λ             // manager of λs
{
  struct Λλ  { λ<Λr> l; λs s; λp p; };
  struct λip { Λ &l; bool operator()(λi a, λi b) const { return l.ls.at(a).p < l.ls.at(b).p; } };

  λip         lp;
  M<λi, Λλ>   ls;    // all λs
  PQ<λi, λip> rq;    // prioritized schedule
  λi          ni{1}; // next λi (always nonzero for managed λ)
  λi          ri{0}; // currently running λi (0 = main thread)

  Λ() : lp{*this}, rq(lp) {}

  bool e (λi i) const { return ls.contains(i); }
  uN   n ()     const { return ls.size(); }
  λi   i ()     const { return ri; }
  λs   si(λi i) const { return ls.at(i).s; }
  λp   pi(λi i) const { return ls.at(i).p; }

  λi   c(λf f, f64 p = 0)              { let   i = ni++;     ls.emplace(i, Λλ{λ<Λr>(f), λR, p}); r(i);                     return i; }
  Λ   &r(λi i, f64 p = NAN, λs s = λR) { auto &l = ls.at(i); if (!std::isnan(p)) l.p = p; if ((l.s = s) == λR) rq.push(i); return *this; }
  Λr   w(λi i)                         { assert(ri != i); let r = ls.at(i).l.result(); ls.erase(i);                        return r; }
  Λ   &x(λi i)                         { assert(ri != i); assert(e(i));                ls.erase(i);                        return *this; }

  Λ &operator<<(λi i)
    { assert(!ri);
      auto &l = ls.at(ri = i); l.l(); ri = 0;
      if (l.l.done()) l.s = λZ;
      return *this; }

  λi operator()()
    { while (!rq.empty()) { let i = rq.top(); rq.pop(); if (e(i) && si(i) == λR) return i; }
      return 0; }
};


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


O &operator<<(O &s, Λ::Λλ const &l)
{
  s << "λ" << l.s;
  return l.s == λZ
    ? s << "=" << l.l.result()
    : s << ":" << l.p;
}


O &operator<<(O &s, Λ const &l)
{
  s << "Λ i=" << l.ri << " q=" << l.rq.size() << std::endl;
  for (let &[k, v] : l.ls) s << "  " << k << "\t" << v << std::endl;
  return s;
}


}

#include "../module/end.hh"


#endif
