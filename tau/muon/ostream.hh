#ifndef tau_muon_ostream_h
#define tau_muon_ostream_h

#include "../debug.hh"
#if tau_debug_iostream || tau_debug_nop


#include "../types.hh"
#include "../utf9.hh"
#include "../flux.hh"


#include "../module/begin.hh"

namespace tau::muon
{


struct ostream
{
  O &s;
  γ &g;

  ostream(Γ &G, O &s_) : s(s_), g(G.γc())
    {
      g.ψw("in"y, α);
      g.λc("main"y, [&]() {
        if (!g.ψφw("in"y)) return 1;
        while (g.ψrw("in"y)) s << g.ψr("in"y) << std::endl;
        return 0;
      });
    }

  ~ostream() { g.g.γx(g.i); }
};


}

#include "../module/end.hh"


#endif
#endif
