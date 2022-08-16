#ifndef tau_flux_Γ_h
#define tau_flux_Γ_h


#include "../types.hh"
#include "types.hh"
#include "Lambda.hh"
#include "theta.hh"
#include "phi.hh"
#include "psi.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct γ;
struct Γ
{
  Λ    l;
  Ψ    q  {l};
  Θ    h  {l};
  Φ    f  {l};
  γi   gi {0};
  bool ye {true};  // yield-enable: if false, calls are atomic
  Θp   t0;

  f64 θp;

  Γ(f64 θp_ = 0.9) : t0(now()), θp(θp_) {}
  ~Γ();

  M<λi, γi> lg;
  M<ψi, γi> qg;
  M<γi, γ*> gs;

  γ &γc();
  Γ &γx(γi);

  Γ         &y  ()              { if (ye && !l.z() && h.y(l.i(), θp)) l.y(λR);                return *this; }
  ΔΘ         Θi ()        const { return now() - t0; }

  λi         λc (γi g, λf &&f)  { y(); let i = l.c(std::move(f)); lg[i] = g; h.c(i);          return i; }
  Λr         λw (γi g, λi i)    { y(); assert(lg.at(i) == g); let r = l.w(i); h.x(i);         return r; }
  Λr         λwi(γi g, λi i)    { y(); assert(lg.at(i) == g);                                 return l.wi(i); }
  Γ         &λx (γi g, λi i)    { y(); assert(lg.at(i) == g); l.x(i); h.x(i); lg.erase(i);    return *this; }
  ΣΘΔ const &λΘ (γi g, λi i)    { y(); assert(lg.at(i) == g);                                 return h.mi(i); }

  ψi    ψc (γi g)               { y(); let i = q.ψc(); qg[i] = g;                             return i; }
  Γ    &ψx (γi g, ψi i)         { y(); assert(qg.at(i) == g); q.ψx(i); qg.erase(i);           return *this; }
  ψi    ψm (γi g, ψi i)         { y(); assert(qg.at(i) == g); let r = q.ψm(i); qg[r] = g;     return r; }
  bool  ψrw(γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.ψrw(i); }
  bool  ψφw(γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.ψφw(i); }
  bool  ψxw(γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.φxw(i); }
  bool  ψri(γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.ψri(i); }
  u9    ψr (γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.ψr(i); }
  bool  ψww(γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.ψww(i); }
  bool  ψwi(γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.ψwi(i);}
  bool  ψw (γi g, ψi i, u9 &&v) { y(); assert(qg.at(i) == g);                                 return q.ψw(i, std::move(v)); }
  bool  ψw (γi g, ψi i, u9c &v) { y(); assert(qg.at(i) == g);                                 return q.ψw(i, v); }
  bool  φe (γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.φe(i); }

  Γ    &Θw (γi g, Θp t)         { y(); assert(lg.at(l.i()) == g); h.w(l.i(), t); l.y(λs::λΘ); return *this; }

  Γ &φc(γi       a, u9c &aq, γi       b, u9c &bq, uN ζc = ζc0);
  Γ &φc(γ const &a, u9c &aq, γ const &b, u9c &bq, uN ζc = ζc0);

  Θp go(F<bool(λi)> const &f = [](λi){ return true; })
    { for (λi t; (t = l()) && f(t);) { h.r(t); l << t; h.s(t); }
      f(0);
      if (let n = h()) { l.r(n, NAN, λR); return h.di(n); }
      else             return never(); }
};


#if tau_debug_iostream
O &operator<<(O &s, Γ const &g)
{
  return s << "Γ TODO";
}
#endif


}

#include "../module/end.hh"


#endif
