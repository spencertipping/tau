#ifndef tau_ϝ_h
#define tau_ϝ_h


#include "gamma.hh"

#include "begin.hh"

namespace tau
{


struct ϝ
{
  γ &g;

  ϝ(γ &g_) : g(g_) { while (g.fs.size() < 4) g.φc(); }
};


}

#include "end.hh"


#endif
