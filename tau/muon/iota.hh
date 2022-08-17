#ifndef tau_muon_iota_h
#define tau_muon_iota_h


#include "base.hh"


#include "../module/begin.hh"

namespace tau::muon
{


struct iota : base
{
  u64c n;

  iota(Γ &G_, u64 n_) : base(G_), n(n_)
    { g.ψw("out"_q, α);
      g.λc("main"_l, [&]() {
        if (!g.ψφw("out"_q)) return 1;
        for (u64 i = 0; i < n; ++i)
          if (g.ψww("out"_q)) g.ψw("out"_q, u9(i));
          else               return 1;
        return 0;
      }); }
};


}

#include "../module/end.hh"


#endif
