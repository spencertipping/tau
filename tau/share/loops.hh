#ifndef τshare_loops_h
#define τshare_loops_h

#include "../Gamma.hh"

#include "../begin.hh"

namespace τ
{


// Flex loop: take an (a → τ[b]) and allow it to apply to the final
// element of a tuple; carry prior elements along.
Γ Γflex(Γ);


Γ Γι(u64 = -1);
Γ Γϊ();
Γ Γtake(u64);
Γ Γdrop(u64);


// Put a τ after each item
Γ Γτ();


// Collect τ-grouped elements into tuple/set/map/etc
Γ Γτt(η0t = η0t::tuple);


// Sum in τ groups
Γ Σi();  // ints
Γ Σf();  // floats


}

#include "../end.hh"

#endif
