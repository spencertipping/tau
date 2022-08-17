#ifndef tau_muon_cat_h
#define tau_muon_cat_h


#include "base.hh"


#include "../module/begin.hh"

namespace tau::muon
{


struct cat : base
{
  γi  a,  b;
  γsi aq, bq;

  cat(Γ &G, γ const &a_, γsi aq_, γ const &b_, γsi bq_, uN c = ζc0)
    : base(G),
      a(a_.i), b(b_.i),
      aq(aq_), bq(bq_)
    { G.φc(a, aq, b, bq, c); }

  ~cat() { G.φx(a, aq, b, bq); }
};


}

#include "../module/end.hh"


#endif
