#ifndef tau_muon_ostream_h
#define tau_muon_ostream_h

#include "../debug.hh"
#if tau_debug_iostream || tau_debug_nop


#include "base.hh"


#include "../module/begin.hh"

namespace tau::muon
{


struct ostream : base
{
  O &s;

  ostream(Γ &G_, O &s_) : base(G_), s(s_)
    {
      g.ψw("in"_q, α);
      g.λc("main"_l, [&]() {
        if (!g.ψφw("in"_q)) return 1;
        while (g.ψrw("in"_q)) s << g.ψr("in"_q) << std::endl;
        return 0;
      });
    }
};


}

#include "../module/end.hh"


#endif
#endif
