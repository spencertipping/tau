#ifndef tau_Λ_h
#define tau_Λ_h


#include <cmath>
#include <random>


#include "arch.hh"
#include "debug.hh"
#include "types.hh"
#include "shd.hh"

#include "lambda.hh"


#if !defined(tau_debug_Λ_randp)
# define tau_debug_Λ_randp 0
#endif


#include "begin.hh"

namespace tau
{


template<class λip> struct Λ_;
struct Λλ;

template<class λip> O &operator<<(O &s, Λ_<λip> &l);
O &operator<<(O &s, Λλ const &l);


struct Λλ
{
  λ<Λr> l;
  λs    s;
  λp    p;
  Θp    y;  // last yield time
  ΔΘ    μ;  // mean quantum duration
  ΣΘΔ   w;  // for profiling (if enabled in Λ)

  Λλ() {}
  Λλ(λf &&f) : l(λ<Λr>(std::move(f))), s(λs::S), p(0), y(now()), μ(1ns) {}

  Λλ &operator=(Λλ &&x)
    { l = std::move(x.l);
      s = x.s;
      p = x.p;
      y = x.y;
      μ = x.μ;
      w = x.w;
      return *this; }
};


sletc λpn = NAN;
ic bool λpm(λp x) { return !std::isnan(x); }

#if tau_compiler != tau_compiler_clang
  static_assert(!λpm(λpn));
#endif


// NOTE: managed λs should yield out with Λ.y
template<class λip>
struct Λ_
{
  λip         lp;
  M<λi, Λλ>   ls;     // all λs
  M<λi, λi>   lz;     // λs awaiting each thing
  PQ<λi, λip> rq;     // prioritized schedule
  S<λi>       srq;    // set of scheduled λis
  λi          ni{0};  // next λi (always nonzero for managed λ)
  λi          ri{0};  // currently running λi (0 = main thread)
  ΣΘΔ         q;      // quantum time measurement

  Λ_(Λ_ &) = delete;
  Λ_() : lp{*this}, rq(lp) {}

  bool e (λi i) const { return ls.contains(i); }
  uN   n ()     const { return ls.size(); }
  λi   i ()     const { return ri; }
  bool z ()     const { return !ri; }
  λs   si(λi i) const { return ls.at(i).s; }
  λp   pi(λi i = 0)   { if (!i) i = (*this)(); return i ? ls.at(i).p : λpn; }

  λi   c(λf &&f, f64 p = 0) { let i = ιi(ni, ls); ls[i] = Λλ(std::move(f)); r(i, p, λs::R);       return  i; }
  Λ_  &x(λi i)              { A(ri != i, "self wait"); A(e(i), "await !e"); ls.erase(i);          return *this; }
  Λ_  &y(λs s)              { A(!z(), "root yield");                        r(ri, NAN, s); λy();  return *this; }

  Λ_  &req(λi i)
    { if (!srq.contains(i)) { rq.push(i); srq.insert(i); }
      return *this; }

  Λ_  &r(λi i, λp p = λpn, λs s = λs::R)
    { if (!e(i)) return *this;
      auto &l = ls.at(i);
      if (λpm(p))             l.p = p;
      if ((l.s = s) == λs::R) req(i);
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

  Λ_ &operator<<(λi i)
    { A(z(), "non-root Λ<<");
      auto &l = ls.at(ri = i);
      q.start(); l.w.start();
      l.l();
      l.w.stop(); l.μ = l.w.μ(); l.y = now(); q.stop();
      ri = 0;
      if (l.l.done())
      { l.s = λs::Z;
        if (lz.contains(i)) r(lz[i], NAN, λs::R); }
      return *this; }

  λi operator()()
    { while (!rq.empty())
      { let i = rq.top();
        srq.erase(i);
        rq.pop();
        if (e(i) && si(i) == λs::R) return i; }
      return 0; }

  Λ_ &go(F<bool(Λ_&, λi)> const &f = [](Λ_&, λi){ return true; })
    { for (λi t; (t = (*this)()) && f(*this, t); *this << t);
      f(*this, 0);
      return *this; }
};


struct λipr
{
  Λ_<λipr> &l;
  std::default_random_engine  g{Sc<uN>(now().time_since_epoch().count())};
  std::bernoulli_distribution d{0.5};
  bool operator()(λi a, λi b) { return d(g); }
};


struct λipf
{
  Λ_<λipf> &l;
  bool operator()(λi a, λi b) const
    { let  t  = now();
      let &la = l.ls.at(a);
      let &lb = l.ls.at(b);
      let  ra = exp(la.p) * (t - la.y) / la.μ;
      let  rb = exp(lb.p) * (t - lb.y) / lb.μ;
      return ra < rb; }
};


typedef Λ_<λipf> Λ;


#if tau_debug_iostream
O &operator<<(O &s, Λλ const &l)
{
  A((l.s == λs::Z) == l.l.done(), "λ/Λ done() desync");
  s << "λ" << l.s;
  return l.s == λs::Z ? s << "=" << l.l.result()
                      : s << ":" << l.p;
}

template<class λip>
O &operator<<(O &s, Λ_<λip> &l)
{
  V<λi> q;
  s << "Λ i=" << l.ri << " rq=";

  while (!l.rq.empty())
  {
    s << l.rq.top() << " ";
    q.push_back(l.rq.top());
    l.rq.pop();
  }
  s << std::endl;

  // Reconstruct the run queue
  for (let i : q) l.rq.push(i);

  for (let &[k, v] : l.ls) s << "  " << k << "\t" << v << std::endl;
  return s;
}
#endif


}

#include "end.hh"


#endif
