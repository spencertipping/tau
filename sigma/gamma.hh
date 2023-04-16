#ifndef σΓ_h
#define σΓ_h

#include "begin.hh"

namespace σ
{


// TODO: synchronize this with doc/sigma.md; that file should drive the stdlib
// design


τ::Ψ0 Γostream_null(τ::O&, τ::St = "");
τ::Ψ2 Γostream_tap (τ::O&, τ::St = "");

τ::Ψ1 Γι(τ::i64 = -1);
τ::Ψ2 Γϊ();

τ::Ψ2 Γtake_first(τ::i64);
τ::Ψ2 Γdrop_first(τ::i64);
τ::Ψ2 Γtake_last (τ::i64);
τ::Ψ2 Γdrop_last (τ::i64);


}

#include "end.hh"

#endif
