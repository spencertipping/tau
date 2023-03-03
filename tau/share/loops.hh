#ifndef τshare_loops_h
#define τshare_loops_h

#include "../Gamma.hh"
#include "topology.hh"

#include "../begin.hh"

namespace τ
{


// Flex loop: take an (a → τ[b]) and allow it to apply to the final
// element of a tuple; carry prior elements along.
Γ Γflex(Γ);


Γffn_ ξι(u64 = -1);
Γffn_ ξϊ();
Γffn_ ξtake(u64);
Γffn_ ξdrop(u64);


// Put a τ after each item
Γffn_ ξτ();


// Collect τ-grouped elements into tuple/set/map/etc
Γffn_ ξτt(η0t = η0t::tuple);


// Sum in τ groups
Γffn_ Σi();  // ints
Γffn_ Σf();  // floats


}

#include "../end.hh"

#endif
