#ifndef τshare_loops_h
#define τshare_loops_h

#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


// Flex loop: take an (a → τ[b]) and allow it to apply to the final
// element of a tuple; carry prior elements along.
Sp<γ> γflex(Sp<γ>);


Sp<γ> γι(uN = -1);
Sp<γ> γϊ();
Sp<γ> γtake(uN);
Sp<γ> γdrop(uN);


}

#include "../end.hh"

#endif
