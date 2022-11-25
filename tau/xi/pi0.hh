#ifndef τξπ0_h
#define τξπ0_h


#include "../pi0.hh"
#include "../pi0-abi1.hh"

#include "begin.hh"

namespace τ::ξ
{


ϝ &pi0(Φ &f, Stc &n, ϝξ t, SP<π0pgm const> p)
{
  return *new ϝ(f, n, t, [&](ϝ &g_)
    { SP<π0hgs> g{new π0hgs(f.ph)};
      (*g)[u9_symbol::str("ϝ")] = f.ph << o9ptr(&g_);
      (*g)[u9_symbol::str("Φ")] = f.ph << o9ptr(&f);
      π0int(π0abi1(), p, f.ph, g).run(); });
}


}

#include "end.hh"


#endif
