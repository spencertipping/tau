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
  bool ye {true};
  Θp   t0;

  f64 θp;

  Γ(f64 θp_ = 0.9) : t0(now()), θp(θp_) {}
  ~Γ();

  M<λi, γi> lg;
  M<ψi, γi> qg;
  M<γi, γ*> gs;

  γ &γc();
  Γ &γx(γi);

  ΔΘ         Θi()         const { return now() - t0; }

  Γ         &y ()               { if (ye && !l.z() && h.y(l.i(), θp)) h.s(l.i()), l.y(λR);    return *this; }

  λi         λc(γi g, λf &&f)   { y(); let i = l.c(std::move(f)); lg[i] = g; h.c(i);          return i; }
  Λr         λw(γi g, λi i)     { y(); assert(lg.at(i) == g); let r = l.w(i); h.x(i);         return r; }
  Γ         &λx(γi g, λi i)     { y(); assert(lg.at(i) == g); l.x(i); h.x(i); lg.erase(i);    return *this; }
  ΣΘΔ const &λΘ(γi g, λi i)     { y(); assert(lg.at(i) == g);                                 return h.mi(i); }

  ψi    ψc (γi g)               { y(); let i = q.ψc(); qg[i] = g;                             return i; }
  Γ    &ψx (γi g, ψi i)         { y(); assert(qg.at(i) == g); q.ψx(i); qg.erase(i);           return *this; }
  bool  ψrw(γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.ψrw(i); }
  bool  ψri(γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.ψri(i); }
  u9    ψr (γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.ψr(i); }
  bool  ψww(γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.ψww(i); }
  bool  ψwi(γi g, ψi i)         { y(); assert(qg.at(i) == g);                                 return q.ψwi(i);}
  bool  ψw (γi g, ψi i, u9 &&v) { y(); assert(qg.at(i) == g);                                 return q.ψw(i, std::move(v)); }
  bool  ψw (γi g, ψi i, u9c &v) { y(); assert(qg.at(i) == g);                                 return q.ψw(i, v); }

  Γ    &Θw (γi g, Θp t)         { y(); assert(lg.at(l.i()) == g); h.w(l.i(), t); l.y(λs::λΘ); return *this; }

  Γ    &go(F<bool(λi)> const &f = [](λi){ return true; })
    { for (λi t; (t = l()) && f(t);)
      {
        std::cout << "THETARUN " << t << std::endl;
        h.r(t);
        std::cout << "theta stopwatches:" << std::endl;
        std::cout << "  m = " << h.mi(t) << std::endl;
        std::cout << "  y = " << h.yi(t) << std::endl;
        std::cout << "RUNNING " << t << std::endl;
        l << t;
        std::cout << "STOPPED " << t << ": " << l.si(t) << std::endl;
        std::cout << "theta stopwatches:" << std::endl;
        std::cout << "  m = " << h.mi(t) << std::endl;
        std::cout << "  y = " << h.yi(t) << std::endl;
        h.s(t);
        std::cout << "THETASTOP " << t << std::endl;

      }
      f(0);
      return *this; }


  Γ &φc(γi       a, u9c &aq, γi       b, u9c &bq, uN ζc = ζc0);
  Γ &φc(γ const &a, u9c &aq, γ const &b, u9c &bq, uN ζc = ζc0);
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
