#ifndef tau_muon_istream_h
#define tau_muon_istream_h

#include "../debug.hh"
#if tau_debug_iostream || tau_debug_nop


#include "base.hh"


#include "../module/begin.hh"

namespace tau::muon
{


struct istream : base
{
  std::istream &s;

  istream(Γ &G_, std::istream &s_) : base(G_), s(s_)
    {
      g.ψw("out"_q, α);
      g.λc("main"_l, [&]() {
        if (!g.ψφw("in"_q)) return 1;

        // FIXME: very broken, as we are sharing memory between
        // entries if the ψ carries more than one
        std::string x;
        while (s >> x)
          if (g.ψww("out"_q)) g.ψw("out"_q, u9(x));
        return 0;
      });
    }
};


}

#include "../module/end.hh"


#endif
#endif
