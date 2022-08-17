#ifndef tau_muon_base_h
#define tau_muon_base_h


#include "../types.hh"
#include "../utf9.hh"
#include "../flux.hh"
#include "../util.hh"


#include "../module/begin.hh"

namespace tau::muon
{


struct base
{
  Γ        &G;
  γ        &g;
  γi const  gi;

  base(Γ &G_) : G(G_), g(G.γc()), gi(g.i) {}
  virtual ~base()                         { G.γx(gi); }

  operator γ const&() const { return g; }
  operator γ      &()       { return g; }
};


}

#include "../module/end.hh"

#endif
