#ifndef σ_h
#define σ_h

#include "tau.hh"
#include "sigma/pre/container.hh"

#include "tau/begin.hh"

namespace σ
{


struct πφ : public τ::πφ_<πφ>,
            τ::φauto_str<πφ>,
            τ::πφP<πφ>,
            τ::φlit<πφ>,
            τ::φbrack<πφ>
{
  using τ::πφ_<πφ>::p;
  using τ::πφP<πφ>::p;
  using τ::φbrack<πφ>::p;
  using τ::φauto_str<πφ>::p;
  using τ::φlit<πφ>::p;
};


struct Γφ : public τ::Γφ_<Γφ, πφ>,
            τ::φauto_str<Γφ>,
            τ::φlit<Γφ>,
            τ::φbrack<Γφ>
{
  using τ::Γφ_<Γφ, πφ>::p;
  using τ::φauto_str<Γφ>::p;
  using τ::φlit<Γφ>::p;
  using τ::φbrack<Γφ>::p;
};


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
