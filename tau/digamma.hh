#ifndef tau_ϝ_h
#define tau_ϝ_h


#include <functional>


#include "types.hh"
#include "gamma.hh"

#include "begin.hh"

namespace tau
{


struct ϝι;
struct ϝϊ;
struct ϝΦ;
struct ϝβ;
struct ϝτ;
struct ϝγ;
struct ϝδ;
struct ϝξ;

typedef F<void(ϝι&)> ϝιf;
typedef F<void(ϝϊ&)> ϝϊf;
typedef F<void(ϝΦ&)> ϝΦf;
typedef F<void(ϝβ&)> ϝβf;
typedef F<void(ϝτ&)> ϝτf;
typedef F<void(ϝγ&)> ϝγf;
typedef F<void(ϝδ&)> ϝδf;
typedef F<void(ϝξ&)> ϝξf;


template<class Ϝ>
struct ϝfr
{
  F<void(Ϝ&)> f;
  template<class... X> int operator()(X... xs) { f(xs...); return 0; }
};


struct ϝι
{
  γ  &g;
  γφ &α, &β, &δ, &ε;

  ϝι(γ &g_, ϝιf &&i, Op<ϝιf> d = std::nullopt)
    : g(g_),
      α(*new γφ(g.f.l, ζb0, false, true)),
      β(*new γφ(g.f.l, ζb0, true, false)),
      δ(*new γφ(g.f.l, ζb0, false, false)),
      ε(*new γφ(g.f.l, ζb0, false, false))
    { g.φc(&α);
      g.φc(&β);
      g.φc(&δ);
      g.φc(&ε);
      g.λc(ϝfr<ϝι>{std::bind(std::move(i), *this)});
      if (d) g.λc(ϝfr<ϝι>{std::bind(std::move(*d), *this)}); }

  γφ::it begin() const { return α.begin(); }
  γφ::it end()   const { return α.end(); }

  template<class T> bool operator<<(T &x) { return β << x; }

  template<class T> T &operator|(T &x) { β(x.α); return x; }
};


}

#include "end.hh"


#endif
