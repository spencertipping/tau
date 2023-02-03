#ifndef τshare_topology_h
#define τshare_topology_h

#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


Sp<γ> γffn(St, F<void(Sp<ψ>, ξi, ξo)>&&);
Sp<γ> γbfn(St, F<void(Sp<ψ>, ξo, ξi)>&&);


}

#include "../end.hh"

#endif
