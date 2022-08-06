#ifndef tau_flux_zeta_h
#define tau_flux_zeta_h


#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>


#include "init.hh"
#include "coro.hh"

#include "../types.hh"
#include "../util/shd.hh"


#include "../module/begin.hh"

namespace tau::flux
{


template<class T>
struct ζ
{
  typedef std::pair<Θp, T> qe;

  ΣΘΔ   rΘ;
  ΣΘΔ   wΘ;
  ΣΘΔ   lΘ;
  uN    c;
  D<qe> xs;

  ζ() {}
  ζ(uN c_) : c(c_) { assert(c); }


  ζ<T> &operator=(ζ<T> &&p)
    { rΘ = p.rΘ;
      wΘ = p.wΘ;
      lΘ = p.lΘ;
      c  = p.c;
      xs = std::move(p.xs);
      return *this; }


  bool ri()    const { return xs.size() > 0; }
  bool wi()    const { return xs.size() < c; }
  u64  rΣ()    const { return lΘ.n; }
  u64  wΣ()    const { return rΣ() + xs.size(); }

  void close()       { c = 0; }
  bool ci()    const { return !c; }
  uN   size()  const { return xs.size(); }


  ΔΘ δ() const
    { if (lΘ.n + xs.size() == 0) return 0ns;
      auto t = 0ns;
      let  n = now();
      for (let &x : xs) t += n - std::get<0>(x);
      return (t + lΘ.Σ()) / (lΘ.n + xs.size()); }

  f64 σ() const
    { return log2(Sf(wΘ.σ().count()))
           - log2(Sf(rΘ.σ().count())); }


  bool write(T const &x)
    { let n = now();
      if (!wi()) return false;
      xs.push_back(qe(n, x));
      return true; }

  T read()
    { assert(ri());
      let x = std::get<1>(xs.front());
      lΘ << now() - std::get<0>(xs.front());
      xs.pop_front();
      return x; }
};


template<class T>
O &operator<<(O &s, ζ<T> const &z)
{
  return s << "ζ["
           << (z.ri() ? "R" : "r")
           << (z.wi() ? "W" : z.ci() ? "#" : "w")
           << " n="  << z.wΣ()
           << " c="  << z.size() << "/" << z.c
           << " rd=" << z.rΘ.σ()
           << " wd=" << z.wΘ.σ()
           << " δ="  << z.δ()
           << " σ="  << z.σ() << "]";
}


}

#include "../module/end.hh"


#endif
