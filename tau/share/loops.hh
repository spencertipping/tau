#ifndef τshare_loops_h
#define τshare_loops_h

#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


// Flex loop: take an (a → τ[b]) and allow it to apply to the final
// element of a tuple; carry prior elements along.
Sp<γ> γflex(Sp<γ>);


Sp<γ> γι(u64 = -1);
Sp<γ> γϊ();
Sp<γ> γtake(u64);
Sp<γ> γdrop(u64);


// Put a τ after each item
Sp<γ> γτ();


// Collect τ-grouped elements into tuple/set/map/etc
Sp<γ> γτt(η0t = η0t::tuple);


}

#include "../end.hh"

#endif
