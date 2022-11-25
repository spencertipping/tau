#ifndef τξπ0_h
#define τξπ0_h


#include "../pi0.hh"
#include "../pi0-abi1.hh"  // NOTE: atypical for τ::ξ

#include "begin.hh"

namespace τ::ξ
{


// NOTE: this entry point is used exactly once within a π₀ context.
// We can't use it to define further ϝs because it binds globals,
// which would interfere with existing ϝs.

ϝ &pi0(Φ &f, Stc &n, ϝξ t, SP<π0pgm const> p, π0bi i = 0)
{
  return *new ϝ(f, n, t, [&, i](ϝ &d)
    { SP<π0hgs> g{new π0hgs(f.ph)};
      (*g)[u9_symbol::str("ϝ₀")] = f.ph << o9ptr(&d);
      (*g)[u9_symbol::str("γ₀")] = f.ph << o9ptr(&d.g);
      (*g)[u9_symbol::str("Φ")]  = f.ph << o9ptr(&f);
      π0int(π0abi1(), p, f.ph, g).run(i); });
}


}

#include "end.hh"


#endif
