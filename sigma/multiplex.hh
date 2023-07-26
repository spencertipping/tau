#ifndef σmultiplex_h
#define σmultiplex_h

#include "begin.hh"

namespace σ
{

using namespace τ;


Γ static_multiplex2(Mc<St, Γ>&, Ψd = Ψd::f);
Γ static_multiplex4(Mc<St, Γ>&, Ψd = Ψd::f);

Γ replicated_multiplex2(Γ, Ψd = Ψd::f);


void Γmultiplex(Γφ&);


}

#include "end.hh"

#endif
