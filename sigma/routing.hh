#ifndef τshare_routing_h
#define τshare_routing_h

#include "../Gamma.hh"

#include "../begin.hh"

namespace τ
{


Γ Γrfn(St, F<void(Ξ&)>&&);

Γ Γswap(St);    // swap named ξd and primary
Γ Γsub(St, Γ);  // run Γ within sub-Ξ


}

#include "../end.hh"

#endif
