#ifndef σΨ2_h
#define σΨ2_h

#include "begin.hh"

namespace σ
{


τ::φ<τ::Ψ2> &φΨ2();


τ::Ψ2 Γg(τ::πf);
τ::Ψ2 Γh();
τ::Ψ2 Γj();  // sorted-join top two Ξ entries
τ::Ψ2 Γk();
τ::Ψ2 Γp(τ::πf);
τ::Ψ2 Γrp(τ::πf);
τ::Ψ2 Γrd(τ::i64);  // drop first N rows
τ::Ψ2 Γrt(τ::i64);  // take first N rows
τ::Ψ2 Γsshc();
τ::Ψ2 Γtcpc();
τ::Ψ2 Γudpc();
τ::Ψ2 Γunixc();
τ::Ψ2 Γw(τ::Γ);
τ::Ψ2 ΓW(τ::Γ);
τ::Ψ2 Γx(τ::St);
τ::Ψ2 Γy(τ::St);

τ::Ψ2 ΓJ(τ::Γ);
τ::Ψ2 ΓM(τ::St);
τ::Ψ2 ΓN();

τ::Ψ2 Γappend(τ::Γ);
τ::Ψ2 Γprepend(τ::Γ);
τ::Ψ2 Γunion(τ::Γ);
τ::Ψ2 Γid();


}

#include "end.hh"

#endif
