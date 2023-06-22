#ifndef σΨ1_h
#define σΨ1_h

#include "begin.hh"

namespace σ
{


τ::φ<τ::Ψ1> φΨ1();

τ::Ψ1 Γf();
τ::Ψ1 Γi(τ::ηmc&);
τ::Ψ1 ΓI(τ::ηmc&);
τ::Ψ1 Γn(τ::i64 = -1);

#if τlinux
τ::Ψ1 ΓrF(τ::fd_t);
τ::Ψ1 Γssh(τ::i16, τ::i32, τ::Γ);
τ::Ψ1 Γtcp(τ::i16, τ::i32, τ::Γ);
τ::Ψ1 Γudp(τ::i16, τ::i32, τ::Γ);
τ::Ψ1 Γunix(τ::St, τ::Γ);
#endif


}

#include "end.hh"

#endif
