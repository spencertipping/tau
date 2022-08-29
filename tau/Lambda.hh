#ifndef tau_flux_Λ_h
#define tau_flux_Λ_h


#include <cmath>


#include "debug.hh"
#include "types.hh"
#include "shd.hh"

#include "lambda.hh"


#include "begin.hh"

#if !defined(tau_debug_flux_Λ_randp)
# define tau_debug_flux_Λ_randp tau_debug
#endif

#if tau_debug_flux_Λ_randp
# include <random>
#endif


namespace tau
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
    Λλ(λf &&f) : l(λ<Λr>(std::move(f))), s(λs::S), p(0) {}

    Λλ &operator=(Λλ &&x)
      { l = std::move(x.l);
        s = x.s;
        p = x.p;
        return *this; }
  };

#if tau_debug_flux_Λ_randp
  struct λip
  { Λ &l;
    std::default_random_engine  g{Sc<uN>(now().time_since_epoch().count())};
    std::bernoulli_distribution d{0.5};
    bool operator()(λi a, λi b) { return d(g); } };
#else
  struct λip
  { Λ &l;
    bool operator()(λi a, λi b) const { return l.pi(a) < l.pi(b); } };
#endif

  λip         lp;
  M<λi, Λλ>   ls;     // all λs
  M<λi, λi>   lz;     // λs awaiting each thing
  PQ<λi, λip> rq;     // prioritized schedule
  λi          ni{0};  // next λi (always nonzero for managed λ)
  λi          ri{0};  // currently running λi (0 = main thread)

  Λ(Λ &) = delete;
  Λ() : lp{*this}, rq(lp) {}

  bool e (λi i) const { return ls.contains(i); }
  uN   n ()     const { return ls.size(); }
  λi   i ()     const { return ri; }
  bool z ()     const { return !ri; }
  λs   si(λi i) const { return ls.at(i).s; }
  λp   pi(λi i = 0)   { if (!i) i = (*this)(); return i ? ls.at(i).p : NAN; }

  λi   c(λf &&f, f64 p = 0) { let   i = ιi(ni, ls); ls[i] = Λλ(std::move(f)); r(i, p, λs::R);       return  i; }
  Λ   &x(λi i)              { assert(ri != i); assert(e(i));                  ls.erase(i);          return *this; }
  Λ   &y(λs s)              { assert(!z());                                   r(ri, NAN, s); λy();  return *this; }

  Λ   &r(λi i, f64 p = NAN, λs s = λs::R)
    { if (!e(i)) return *this;
      auto &l = ls.at(i);
      if (!std::isnan(p)) l.p = p;
      if ((l.s = s) == λs::R) rq.push(i);
      return *this; }

  bool wi(λi i) { return si(i) == λs::Z; }
  Λr   w (λi i)
    { assert(ri != i);  // λ cannot await itself
      assert(!z() || wi(i));
      while (!wi(i))
      { assert(!lz.contains(i));
        lz[i] = ri;
        y(λs::W); }
      lz.erase(ri);
      let r = ls.at(i).l.result();
      ls.erase(i);
      return r; }

  Λ &operator<<(λi i)
    { assert(z());
      auto &l = ls.at(ri = i); l.l(); ri = 0;
      if (l.l.done())
      { l.s = λs::Z;
        if (lz.contains(i)) r(lz[i], NAN, λs::R); }
      return *this; }

  λi operator()()
    { while (!rq.empty())
      { let i = rq.top();
        if (e(i) && si(i) == λs::R) return i;
        else                        rq.pop(); }
      return 0; }

  Λ &go(F<bool(Λ&, λi)> const &f = [](Λ&, λi){ return true; })
    { for (λi t; (t = (*this)()) && f(*this, t); *this << t);
      f(*this, 0);
      return *this; }
};


#if tau_debug_iostream
O &operator<<(O &s, Λ::Λλ const &l)
{
  assert((l.s == λs::Z) == l.l.done());
  s << "λ" << l.s;
  return l.s == λs::Z ? s << "=" << l.l.result()
                      : s << ":" << l.p;
}


O &operator<<(O &s, Λ const &l)
{
  s << "Λ i=" << l.ri << " q=" << l.rq.size() << std::endl;
  for (let &[k, v] : l.ls) s << "  " << k << "\t" << v << std::endl;
  return s;
}
#elif tau_debug_nop
O &operator<<(O &s, Λ::Λλ const &l) { return s; }
O &operator<<(O &s, Λ     const &l) { return s; }
#endif


}

#include "end.hh"


#endif
