#ifndef τshare_loops_h
#define τshare_loops_h

#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


// Flex loop: take an (a → τ[b]) and allow it to apply to the final
// element of a tuple; carry prior elements along.
γ γflex(γ);


γ γι(u64 = -1);
γ γϊ();
γ γtake(u64);
γ γdrop(u64);


// Put a τ after each item
γ γτ();


// Collect τ-grouped elements into tuple/set/map/etc
γ γτt(η0t = η0t::tuple);


// Sum in τ groups
γ Σi();  // ints
γ Σf();  // floats


}

#include "../end.hh"

#endif
