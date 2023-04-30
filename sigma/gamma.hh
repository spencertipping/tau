#ifndef σΓ_h
#define σΓ_h

#include "auto.hh"

#include "psi0.hh"
#include "psi1.hh"
#include "psi2.hh"
#include "psi4.hh"

#include "begin.hh"

namespace σ
{


// FIXME: we don't need to define these parsers; instead, we should provide
// components for core Γφ()

τ::φ<τ::Γ> φΓ();
τ::φ<τ::Γ> φΓa();


τ::Γ Γvs(σident);
τ::Γ Γvg(σident);

τ::Γ Γdef(σident, τ::Γ);
τ::Γ Γref(σident);
τ::Γ Γsmux(τ::Mc<τ::St, τ::Γ>&);
τ::Γ Γsdemux();
τ::Γ Γdmux(τ::Γ);
τ::Γ Γddemux();
τ::Γ Γconn(τ::St);
τ::Γ Γsdef(τ::St, τ::Γ);

τ::Γ Γpush();
τ::Γ Γdrop();
τ::Γ Γdebug(σident);


}

#include "end.hh"

#endif
