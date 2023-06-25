#ifndef σ_h
#define σ_h

#include "tau.hh"
#include "tau/begin.hh"

namespace σ
{


typedef τ::πφ<τ::πφP, τ::φlit, τ::φbrack> πφ;
typedef τ::Γφ<πφ,     τ::φlit, τ::φbrack> Γφ;

typedef τ::τe τe;
typedef τ::Ξ  Ξ;
typedef τ::Γ  Γ;

typedef τ::ψ    ψ;
typedef τ::Ψ0   Ψ0;
typedef τ::Ψ1   Ψ1;
typedef τ::Ψ2   Ψ2;
typedef τ::Ψ4   Ψ4;
typedef τ::Ψaux Ψaux;

typedef τ::ξi ξi;
typedef τ::ξo ξo;
typedef τ::ξd ξd;


// Create the core σ grammar to parse Γ expressions. This will add
// all platform-compatible operators to the result (native + shared).
Γφ σΓ();


}

#include "tau/end.hh"

#endif
