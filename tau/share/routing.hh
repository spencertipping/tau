#ifndef τshare_routing_h
#define τshare_routing_h

#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


γ γrfn(St, F<void(Ξ&)>&&);

γ γswap(St);        // swap named ξd and primary
γ γsub(St, γ);  // run γ within sub-Ξ


// Apply a forward-oriented γ to the reverse (backward) pipeline
γ γb(γ);

// Swap forward and backward ξs within the primary ξd
γ γξswap();


}

#include "../end.hh"

#endif
