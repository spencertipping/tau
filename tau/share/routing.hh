#ifndef τshare_routing_h
#define τshare_routing_h

#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


γ γrfn(St, F<void(Ξ&)>&&);

γ γswap(St);    // swap named ξd and primary
γ γsub(St, γ);  // run γ within sub-Ξ


}

#include "../end.hh"

#endif
