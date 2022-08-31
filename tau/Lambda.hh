#ifndef tau_Λ_h
#define tau_Λ_h


#include <cmath>


#include "debug.hh"
#include "types.hh"
#include "shd.hh"

#include "lambda.hh"


#include "begin.hh"

#if !defined(tau_debug_Λ_randp)
# define tau_debug_Λ_randp 0
#endif

#if !defined(tau_trace_Λ_switches)
# define tau_trace_Λ_switches 0
#endif


#if tau_debug_Λ_randp
# include <random>
#endif


namespace tau
{


struct Λ;
struct Λλ;

O &operator<<(O &s, Λ const &l);
O &operator<<(O &s, Λλ const &l);


struct Λλ
{
  λ<Λr> l;
  λs    s;
  λp    p;
  ΣΘΔ   w;  // for profiling

  Λλ() {}
  Λλ(λf &&f) : l(λ<Λr>(std::move(f))), s(λs::S), p(0) {}

  Λλ &operator=(Λλ &&x)
    { l = std::move(x.l);
      s = x.s;
      p = x.p;
      w = x.w;
      return *this; }
};


sletc λpn = NAN;
ic bool λpm(λp x) { return !std::isnan(x); }

static_assert(!λpm(λpn));


// NOTE: managed λs should yield out with Λ.y
struct Λ
{
#if tau_debug_Λ_randp
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

  // TODO: fair queueing -- right now the priority queue starves
  // some λs

  λip         lp;
  M<λi, Λλ>   ls;     // all λs
  M<λi, λi>   lz;     // λs awaiting each thing
  PQ<λi, λip> rq;     // prioritized schedule
  λi          ni{0};  // next λi (always nonzero for managed λ)
  λi          ri{0};  // currently running λi (0 = main thread)

  bool        prof = false;

  Λ(Λ &) = delete;
  Λ() : lp{*this}, rq(lp) {}

  bool e (λi i) const { return ls.contains(i); }
  uN   n ()     const { return ls.size(); }
  λi   i ()     const { return ri; }
  bool z ()     const { return !ri; }
  λs   si(λi i) const { return ls.at(i).s; }
  λp   pi(λi i = 0)   { if (!i) i = (*this)(); return i ? ls.at(i).p : λpn; }

  λi   c(λf &&f, f64 p = 0) { let i = ιi(ni, ls); ls[i] = Λλ(std::move(f)); r(i, p, λs::R);       return  i; }
  Λ   &x(λi i)              { A(ri != i, "self wait"); A(e(i), "await !e"); ls.erase(i);          return *this; }
  Λ   &y(λs s)              { A(!z(), "root yield");                        r(ri, NAN, s); λy();  return *this; }

  Λ   &r(λi i, λp p = λpn, λs s = λs::R)
    { if (!e(i)) return *this;
      auto &l = ls.at(i);
      if (λpm(p))             l.p = p;
      if ((l.s = s) == λs::R) rq.push(i);
      return *this; }

  bool wi(λi i) { return si(i) == λs::Z; }
  Λr   w (λi i)
    { A(ri != i,       "λw self");
      A(!z() || wi(i), "root λw non-Z");
      while (!wi(i))
      { A(!lz.contains(i), "multiple co-awaiting " << i);
        lz[i] = ri;
        y(λs::W); }
      lz.erase(ri);
      let r = ls.at(i).l.result();
      ls.erase(i);
      return r; }

  Λ &operator<<(λi i)
    { A(z(), "non-root Λ<<");
      if constexpr (tau_trace_Λ_switches) std::cout << "Λ << " << i << std::endl;
      auto &l = ls.at(ri = i);
      if (prof) l.w.start();
      l.l();
      if (prof) l.w.stop();
      ri = 0;
      if constexpr (tau_trace_Λ_switches) std::cout << *this << std::endl;
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
O &operator<<(O &s, Λλ const &l)
{
  A((l.s == λs::Z) == l.l.done(), "λ/Λ done() desync");
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
#endif


}

#include "end.hh"


#endif
