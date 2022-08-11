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


// NOTE: managed λs should yield out with Λ.y
struct Λ
{
  struct Λλ
  {
    λ<Λr> l;
    λs    s;
    λp    p;

    Λλ() {}
    Λλ(λf &&f) : l(λ<Λr>(std::move(f))), s(λS), p(0) {}

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
  M<λi, Λλ>   ls;     // all λs
  M<λi, λi>   lz;     // λs awaiting each thing
  PQ<λi, λip> rq;     // prioritized schedule
  λi          ni{0};  // next λi (always nonzero for managed λ)
  λi          ri{0};  // currently running λi (0 = main thread)

  Λ() : lp{*this}, rq(lp) {}

  bool e (λi i) const { return ls.contains(i); }
  uN   n ()     const { return ls.size(); }
  λi   i ()     const { return ri; }
  bool z ()     const { return !ri; }
  λs   si(λi i) const { return ls.at(i).s; }
  λp   pi(λi i = 0)   { if (!i) i = (*this)(); return i ? ls.at(i).p : NAN; }

  λi   c(λf &&f, f64 p = 0)            { let   i = ιi(ni, ls); ls[i] = Λλ(std::move(f)); r(i, p, λR);                      return  i; }
  Λ   &r(λi i, f64 p = NAN, λs s = λR) { auto &l = ls.at(i); if (!std::isnan(p)) l.p = p; if ((l.s = s) == λR) rq.push(i); return *this; }
  Λ   &x(λi i)                         { assert(ri != i); assert(e(i));                   ls.erase(i);                     return *this; }
  Λ   &y(λs s)                         {                                                                                                   if (!z()) r(ri, NAN, s); λy();   return *this; }

  bool wi(λi i) { return si(i) == λZ; }
  Λr   w (λi i)
    { assert(ri != i);  // λ cannot await itself
      assert(!z() || wi(i));
      while (!wi(i))
      { assert(!lz.contains(i));
        lz[i] = ri;
        y(λW); }
      lz.erase(ri);
      let r = ls.at(i).l.result();
      ls.erase(i);
      return r; }

  Λ &operator<<(λi i)
    { assert(z());
      auto &l = ls.at(ri = i); l.l(); ri = 0;
      if (l.l.done())
      { l.s = λZ;
        if (lz.contains(i)) r(lz[i], NAN, λR); }
      return *this; }

  λi operator()()
    { while (!rq.empty())
      { let i = rq.top();
        if (e(i) && si(i) == λR) return i;
        else                     rq.pop(); }
      return 0; }

  Λ &go(F<bool(Λ&, λi)> const &f = [](Λ&, λi){ return true; })
    { for (λi t; (t = (*this)()) && f(*this, t); *this << t);
      f(*this, 0);
      return *this; }
};


#if tau_debug_iostream
O &operator<<(O &s, Λ::Λλ const &l)
{
  assert((l.s == λZ) == l.l.done());
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
#endif


}

#include "../module/end.hh"


#endif
