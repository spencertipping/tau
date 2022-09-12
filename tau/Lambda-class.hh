#ifndef tau_Λ_class_h
#define tau_Λ_class_h


#include "arch.hh"
#include "debug.hh"
#include "types.hh"
#include "shd.hh"

#include "lambda.hh"


#include "begin.hh"

namespace tau
{


template<class Λp>
struct Λt
{
  λ<Λr>           l;
  λs              s;
  typename Λp::p  p;   // priority
  typename Λp::ps ps;  // priority state
  λt              t;   // tag (for debugging/tracking)

  Λt() {}
  Λt(λf &&f, λt t_ = 0) : l(λ<Λr>(std::move(f))), s(λs::S), t(t_) {}

  Λt &operator=(Λt &&x)
    { l  = std::move(x.l);
      s  = x.s;
      p  = x.p;
      ps = x.ps;
      t  = x.t;
      return *this; }
};


// NOTE: managed λs should yield out with Λ.y, not λy() defined by lambda.hh
template<class Λp>
struct Λ_
{
  M<λi, Λt<Λp>> ls;     // all λs
  Λp            rq;
  λi            ni{0};  // next λi (always nonzero for managed λ)
  λi            ri{0};  // currently running λi (0 = main thread)
  ΣΘΔ           qΘ;     // quantum time measurement

  Λ_(Λ_ &)  = delete;
  Λ_(Λ_ &&) = delete;
  Λ_() : rq{*this} {}

  bool e (λi i) const { return ls.contains(i); }
  λs   si(λi i) const { return ls.at(i).s; }
  uN   n ()     const { return ls.size(); }
  λi   i ()     const { return ri; }
  bool z ()     const { return !ri; }

  λi   c(λf &&f, λt t = 0, f64 p = 0)
    { let i = ιi(ni, ls);
      ls[i] = Λt<Λp>(std::move(f), t);
      r(i, p, λs::R);
      return  i; }

  Λ_  &x(λi i)
    { A(ri != i, "self λx"); A(e(i), "λx !e");
      ls.erase(i);
      return *this; }

  Λ_  &y(λs s)
    { A(!z(), "root yield");
      r(ri, NAN, s);
      λy();
      return *this; }

  Λ_  &r(λi i, typename Λp::p p = Λp::λpn, λs s = λs::R)
    { if (!e(i)) return *this;
      auto &l = ls.at(i);
      if (Λp::λpm(p))         l.p = p;
      if ((l.s = s) == λs::R) rq << i;
      return *this; }

  Λ_ &operator<<(λi i)
    { A(z(), "non-root Λ<<");
      auto &l = ls.at(ri = i);
      qΘ.start();
      l.ps.start();
      l.l();
      l.ps.stop();
      ri = 0;
      if (l.l.done()) ls.erase(i);
      qΘ.stop();
      return *this; }

  λi operator()()
    { while (rq)
      { let i = rq();
        if (e(i) && si(i) == λs::R) return i; }
      return 0; }

  Λ_ &go(F<bool(Λ_&, λi)> const &f = [](Λ_&, λi){ return true; })
    { for (λi t; (t = (*this)()) && f(*this, t); *this << t);
      f(*this, 0);
      return *this; }
};


#if tau_debug_iostream
template<class Λp>
O &operator<<(O &s, Λt<Λp> const &l)
{
  return s << "λ" << l.s << "@" << l.t << ":" << l.p << " " << l.ps;
}

template<class Λp>
O &operator<<(O &s, Λ_<Λp> &l)
{
  s << "Λ i=" << l.ri << " rq=" << l.rq << std::endl;
  for (let &[k, v] : l.ls) s << "  " << k << "\t" << v << std::endl;
  return s;
}
#endif


}

#include "end.hh"


#endif
