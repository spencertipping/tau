#ifndef σquote_h
#define σquote_h

#include "../sigma.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


Γ quote  (Ψd = Ψd::f);
Γ unquote(Ψd = Ψd::f);

void Γquote(Γφ&);

}

#include "end.hh"

#endif
