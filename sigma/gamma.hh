#ifndef σΓ_h
#define σΓ_h

#include "psi0.hh"
#include "psi1.hh"
#include "psi2.hh"
#include "psi4.hh"

#include "begin.hh"

namespace σ
{


τ::φ<τ::Γ> φΓa();
τ::φ<τ::Γ> φΓ();


τ::Γ Γdef(τ::St, τ::Γ);
τ::Γ Γref(τ::St);
τ::Γ Γsmux(τ::Mc<τ::St, τ::Γ>&);
τ::Γ Γsdemux();
τ::Γ Γdmux(τ::Γ);
τ::Γ Γddemux();
τ::Γ Γconn(τ::St);
τ::Γ Γsdef(τ::St, τ::Γ);

τ::Γ Γpush();
τ::Γ Γdrop();
τ::Γ Γdebug(τ::St);


}

#include "end.hh"

#endif
