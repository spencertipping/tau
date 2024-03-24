#ifndef σquote_h
#define σquote_h

#include "../sigma-pre.hh"
#include "begin.hh"

namespace σ
{


Γ quote  (τ::Ψd = τ::Ψd::f);
Γ unquote(τ::Ψd = τ::Ψd::f);

void Γquote(Γφ&);

}

#include "end.hh"

#endif
