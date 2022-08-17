#ifndef tau_muon_iota_h
#define tau_muon_iota_h


#include "../types.hh"
#include "../utf9.hh"
#include "../flux.hh"


#include "../module/begin.hh"

namespace tau::muon
{


struct iota
{
  γ    &g;
  u64c  n;

  iota(Γ &G, u64 n_) : g(G.γc()), n(n_)
    {
      g.ψw("out"y, α);
      g.λc("main"y, [&]() {
        if (!g.ψφw("out"y)) return 1;
        for (u64 i = 0; i < n; ++i)
          if (g.ψww("out"y)) g.ψw("out"y, u9(i));
        return 0;
      });
    }

  // FIXME: destructor ordering matters; g may or may not be valid here
  // we probably want γs to be externally owned
  //
  // On that point, we probably want γ to be {Γ*, γi} or similar
  ~iota() { g.g.γx(g.i); }
};


}

#include "../module/end.hh"


#endif
