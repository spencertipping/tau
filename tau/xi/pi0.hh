#ifndef τξπ0_h
#define τξπ0_h


#include "../pi0.hh"

#include "begin.hh"

namespace τ::ξ
{


ϝ &pi0(Φ &f, Stc &n, ϝξ t, SP<π0pgm const> p)
{
  return *new ϝ(f, n, t, [&](ϝ &g)
    { // TODO: bind ϝ into π₀int for ϝ API (as a global)
      π0int(π0abi1(), p, f.ph, SP<π0hgs>(new π0hgs{f.ph})).run(); });
}


}

#include "end.hh"


#endif
