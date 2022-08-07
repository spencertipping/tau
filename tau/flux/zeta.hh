#ifndef tau_flux_zeta_h
#define tau_flux_zeta_h


#include <cmath>
#include <utility>


#include "../types.hh"
#include "../util/shd.hh"

#include "types.hh"


#include "../module/begin.hh"

namespace tau::flux
{


template<class T>
struct ζ
{
  ΣΘΔ         lΘ;
  uN          c;
  D<P<Θp, T>> xs;

  ζ(uN c_ = 64) : c(c_) { assert(c); }


  ζ<T> &operator=(ζ<T> &&p)
    { lΘ = p.lΘ;
      c  = p.c;
      xs = std::move(p.xs);
      return *this; }


  bool ri()    const { return xs.size() > 0; }
  bool wi()    const { return xs.size() < c; }
  u64  rΣ()    const { return lΘ.n; }
  u64  wΣ()    const { return rΣ() + xs.size(); }

  void x()           { c = 0; }
  bool ci()    const { return !c; }
  uN   n()     const { return xs.size(); }


  // Mean per-item latency through this pipe
  ΔΘ δ() const
    { if (lΘ.n + xs.size() == 0) return 0ns;
      auto t = 0ns;
      let  n = now();
      for (let &x : xs) t += n - std::get<0>(x);
      return (t + lΘ.Σ()) / (lΘ.n + xs.size()); }


  bool w(T const &x)
    { let n = now();
      if (!wi()) return false;
      xs.push_back(std::make_pair(n, x));
      return true; }

  bool w(T &&x)
    { let n = now();
      if (!wi()) return false;
      xs.push_back(std::make_pair(n, std::move(x)));
      return true; }

  T r()
    { assert(ri());
      let [t, x] = xs.front();
      lΘ << now() - t;
      xs.pop_front();
      return x; }
};


#if tau_debug_iostream
template<class T>
O &operator<<(O &s, ζ<T> const &z)
{
  return s << "ζ["
           << (z.ri() ? "R" : "r")
           << (z.wi() ? "W" : z.ci() ? "#" : "w")
           << " n=" << z.wΣ()
           << " c=" << z.n() << "/" << z.c
           << " δ=" << z.δ() << "]";
}
#endif


}

#include "../module/end.hh"


#endif
