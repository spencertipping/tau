#ifndef τshare_routing_h
#define τshare_routing_h

#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


Sp<γ> γrfn(St, F<void(Ξ&)>&&);

Sp<γ> γswap(St);        // swap named ξd and primary
Sp<γ> γsub(St, Sp<γ>);  // run γ within sub-Ξ


// Apply a forward-oriented γ to the reverse (backward) pipeline
Sp<γ> γb(Sp<γ>);

// Swap forward and backward ξs within the primary ξd
Sp<γ> γξswap();


}

#include "../end.hh"

#endif
