#ifndef tau_flux_ζ_h
#define tau_flux_ζ_h


#include <cmath>
#include <utility>


#include "../types.hh"
#include "../util/shd.hh"

#include "types.hh"


#include "../module/begin.hh"

namespace tau::flux
{


template<class T, class S>
struct ζ
{
  S           s;
  ΣΘΔ         lΘ;
  uN          c;
  uN          Σs;
  u64         Σw;
  D<P<Θp, T>> xs;

  ζ(uN c_ = ζc0) : c(c_), Σs(0), Σw(0) { assert(c); }


  ζ &operator=(ζ &&p)
    { s  = p.s;
      lΘ = p.lΘ;
      c  = p.c;
      Σs = p.Σs;
      Σw = p.Σw;
      xs = std::move(p.xs);
      return *this; }


  bool  ri()   const {        return xs.size() > 0; }
  bool  wi()   const {        return Σs < c; }
  u64   rΣ()   const {        return Σw - Σs; }
  u64   wΣ()   const {        return Σw; }

  ζ    &x ()         { c = 0; return *this; }
  bool  xi()   const {        return !c; }


  ΔΘ μΘ() const
    { if (!Σw) return 0ns;
      auto t = 0ns;
      let  n = now();
      for (let &x : xs) t += n - std::get<0>(x);
      return (t + lΘ.Σ()) / (lΘ.n + xs.size()); }


  bool w(T const &x, bool force = false, bool front = false)
    { let n = now();
      if (!force && !wi()) return false;
      let s_ = s(x);
      assert(s_);
      Σs += s_;
      Σw += s_;
      if (front) xs.push_front(std::make_pair(n, x));
      else       xs.push_back (std::make_pair(n, x));
      return true; }

  bool w(T &&x, bool force = false, bool front = false)
    { let n = now();
      if (!force && !wi()) return false;
      let s_ = s(x);
      assert(s_);
      Σs += s_;
      Σw += s_;
      if (front) xs.push_front(std::make_pair(n, std::move(x)));
      else       xs.push_back (std::make_pair(n, std::move(x)));
      return true; }

  T r()
    { assert(ri());
      let [t, x] = xs.front();
      lΘ << now() - t;
      Σs -= s(x);
      xs.pop_front();
      return x; }
};


#if tau_debug_iostream
template<class T, class S>
O &operator<<(O &s, ζ<T, S> const &z)
{
  return s << "ζ["
           << (z.ri() ? "R" : "r")
           << (z.wi() ? "W" : z.xi() ? "#" : "w")
           << " w=" << z.wΣ()
           << " s=" << z.Σs << "/" << z.c
           << " μΘ=" << z.μΘ() << "]";
}
#endif


}

#include "../module/end.hh"


#endif
