#ifndef tau_flux_Λ_h
#define tau_flux_Λ_h


#include <cmath>


#include "../types.hh"
#include "../utf9.hh"

#include "types.hh"
#include "lambda.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct λc            // async blocking condition
{
  bool i = false;
  λc &a() { i = true;        return *this; }  // awaken
  λc &w() { while (!i) λy(); return *this; }  // wait
};


// NOTE: managed λs should yield out with tau::flux::λy()
struct Λ             // manager of λs
{
  struct Λλ
  {
    λ<Λr> l;
    λs    s;
    λp    p;

    Λλ() {}
    Λλ(λf f) : l(λ<Λr>(f)), s(λS), p(0) {}

    Λλ &operator=(Λλ &&x)
      { l = std::move(x.l);
        s = x.s;
        p = x.p;
        return *this; }
  };

  struct λip
  { Λ &l;
    bool operator()(λi a, λi b) const { return l.ls.at(a).p < l.ls.at(b).p; } };

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
  λp   pi(λi i = 0)   { if (!i) i = (*this)(); return i ? ls.at(i).p : NAN; }

  λi   c(λf f, f64 p = 0)              { let   i = ni++;     ls[i] = Λλ(f); r(i, p, λR);                            return i; }
  Λ   &r(λi i, f64 p = NAN, λs s = λR) { auto &l = ls.at(i); if (!std::isnan(p)) l.p = p; if ((l.s = s) == λR) rq.push(i); return *this; }
  Λr   w(λi i)                         { assert(ri != i); let r = ls.at(i).l.result(); ls.erase(i);                        return r; }
  Λ   &x(λi i)                         { assert(ri != i); assert(e(i));                ls.erase(i);                        return *this; }

  Λ &operator<<(λi i)
    { assert(!ri);
      auto &l = ls.at(ri = i); l.l(); ri = 0;
      if (l.l.done()) l.s = λZ;
      return *this; }

  λi operator()()
    { while (!rq.empty())
      { let i = rq.top();
        if (e(i) && si(i) == λR) return i;
        else                     rq.pop(); }
      return 0; }

  // NOTE: not used by Γ, as this doesn't provide timing insight
  Λ &go(bool(*f)() = [](){ return true; })
    { for (λi t; f() && (t = (*this)()); *this << t);
      return *this; }
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
  return l.s == λZ ? s << "=" << l.l.result()
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
