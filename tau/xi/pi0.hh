#ifndef τξπ0_h
#define τξπ0_h


#include "../pi0.hh"

#include "begin.hh"

namespace τ::ξ
{


ϝ &pi0(Φ &f, Stc &n, ϝξ t, SP<π0pgm const> p)
{
  // TODO: bind ϝ into π₀int for ϝ API
  return *new ϝ(f, n, t, [&](ϝ &g) { π0int(π0abi1(), p, f.ph).run(); });
}


}

#include "end.hh"


#endif
