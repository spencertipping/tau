#ifndef σΓ_h
#define σΓ_h

#include "psi0.hh"
#include "psi1.hh"
#include "psi2.hh"
#include "psi4.hh"

#include "begin.hh"

namespace σ
{


τ::Γ Γvs(τ::πident);
τ::Γ Γvg(τ::πident);

τ::Γ Γdef(τ::πident, τ::Γ);
τ::Γ Γref(τ::πident);
τ::Γ Γsmux(τ::Mc<τ::St, τ::Γ>&);
τ::Γ Γsdemux();
τ::Γ Γdmux(τ::Γ);
τ::Γ Γddemux();
τ::Γ Γconn(τ::St);
τ::Γ Γsdef(τ::St, τ::Γ);

τ::Γ Γpush();
τ::Γ Γdrop();
τ::Γ Γdebug(τ::πident);


}

#include "end.hh"

#endif
