#ifndef σΓ_h
#define σΓ_h

#include "begin.hh"

namespace σ
{


τ::φ<τ::Γ> φΓ();


τ::Γ Γdef(τ::St, τ::Γ);
τ::Γ Γref(τ::St);
τ::Γ Γconn(τ::St);
τ::Γ Γdefs(τ::St, τ::Γ);

τ::Γ Γpush();
τ::Γ Γdrop();


}

#include "end.hh"

#endif
