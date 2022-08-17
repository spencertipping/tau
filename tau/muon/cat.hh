#ifndef tau_muon_cat_h
#define tau_muon_cat_h


#include "../types.hh"
#include "../utf9.hh"
#include "../flux.hh"
#include "../util.hh"


#include "../module/begin.hh"

namespace tau::muon
{


struct cat
{
  γ   &g;
  γ   &a, &b;
  u9c aq, bq;

  cat(Γ &G, γ &a_, u9c &aq_, γ &b_, u9c &bq_, uN c = ζc0)
    : g(G.γc()),
      a(a_),   b(b_),
      aq(aq_), bq(bq_)
    { G.φc(a.i, aq, b.i, bq, c); }

  ~cat() { if (a.φe(aq)) a.ψw(aq, ω);
           if (b.φe(bq)) b.ψw(bq, ω); }
};


}

#include "../module/end.hh"


#endif
