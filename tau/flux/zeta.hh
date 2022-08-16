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


template<class T>
struct ζe
{
  T  x;
  uN s;
  Θp h;
};


template<class T, class S>
struct ζ
{
  S        s;
  ΣΘΔ      lΘ;
  uN       c;
  uN       Σs;
  u64      Σw;
  D<ζe<T>> xs;

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
      for (let &x : xs) t += n - x.h;
      return (t + lΘ.Σ()) / (lΘ.n + xs.size()); }


  bool w(T const &x, bool force = false, bool front = false)
    { let n = now();
      if (!force && !wi()) return false;
      let s_ = s(x);
      assert(s_);
      Σs += s_;
      Σw += s_;
      if (front) xs.push_front(ζe<T>{x, s_, n});
      else       xs.push_back (ζe<T>{x, s_, n});
      return true; }

  bool w(T &&x, bool force = false, bool front = false)
    { let n = now();
      if (!force && !wi()) return false;
      let s_ = s(x);
      assert(s_);
      Σs += s_;
      Σw += s_;
      if (front) xs.push_front(ζe<T>{std::move(x), s_, n});
      else       xs.push_back (ζe<T>{std::move(x), s_, n});
      return true; }

  T r()
    { assert(ri());
      let x = std::move(xs.front());
      lΘ << now() - x.h;
      Σs -= x.s;
      xs.pop_front();
      return x.x; }
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
#elif tau_debug_nop
template<class T, class S>
O &operator<<(O &s, ζ<T, S> const &z) { return s; }
#endif


}

#include "../module/end.hh"


#endif
