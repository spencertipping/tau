#ifndef τΛ_prio_h
#define τΛ_prio_h


#include <cmath>


#include "arch.hh"
#include "debug.hh"
#include "types.hh"
#include "shd.hh"

#include "lambda.hh"
#include "Lambda-class.hh"


#include "begin.hh"

namespace τ
{


struct Λpf            // fair priority scheduler
{
  struct ps
  {
    ΔΘ  μ = Θq;       // initially, assume a minimal mean quantum
    Θp  y = never();  // initially, this λ has never yielded
    ΣΘΔ w;

    void start() { w.start(); }
    void stop () { w.stop(); y = now(); μ = w.μ(); }

    f64 operator()(f64 p, Θp t) const
      { return exp(p) * (t - y) / μ; }
  };

  struct po
  {
    Λ_<Λpf> &l;
    bool operator()(λi a, λi b) const
      { let  t  = now();
        let &la = l.ls.at(a);
        let &lb = l.ls.at(b);
        return la.ps(la.p, t) < lb.ps(lb.p, t); }
  };


  typedef f64 p;

  sletc λpn = NAN;
  static constexpr bool λpm(p x) { return !std::isnan(x); }


  Λ_<Λpf>    &l;
  po          o;
  PQ<λi, po>  q;
  S<λi>       e;

  Λpf(Λ_<Λpf> &l_) : l{l_}, o{l}, q{o} {}

  operator   bool() const { return !q.empty(); }
  λi   operator()()       { let x = q.top(); q.pop(); e.erase(x); return x; }
  Λpf &operator<<(λi x)
    { if (!e.contains(x)) { e.insert(x); q.push(x); }
      return *this; }
};


#if tau_debug_iostream
O &operator<<(O &s, Λpf::ps const &p)
{
  return s << "Λpf::ps μ=" << p.μ << " y=" << p.y;
}

O &operator<<(O &s, Λpf &q)
{
  return s << "TODO: <<Λpf";
}
#endif


}

#include "end.hh"


#endif
