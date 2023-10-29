#ifndef σ_h
#define σ_h

#include "tau.hh"
#include "sigma/pre-kv.hh"
#include "sigma/pre-container.hh"
#include "sigma/pre-container-search.hh"
#include "sigma/pre-container-kv.hh"
#include "sigma/pre-multiplex.hh"
#include "sigma/pre-sstream.hh"

#include "tau/begin.hh"

namespace σ
{


// NOTE: π parsers can inline Γ values, hence recursive templates

template<class Γφ>
struct πφ__ : public τ::πφ_<πφ__<Γφ>>,
  τ::φauto_str<πφ__<Γφ>>,
  τ::πφP<πφ__<Γφ>>,
  τ::φlit<πφ__<Γφ>>,
  τ::φbrack<πφ__<Γφ>>,
  σ::pre::at_parsers<πφ__<Γφ>>,
  σ::pre::at_ct_parsers<πφ__<Γφ>>,
  σ::pre::at_cb_parsers<πφ__<Γφ>>,
  σ::pre::mux_parsers<πφ__<Γφ>>
{
  Γφ &g;
  πφ__(Γφ &g_) : g(g_) {}

  using T = πφ__<Γφ>;

  using τ::πφ_<T>::p;
  using τ::πφP<T>::p;
  using τ::φbrack<T>::p;
  using τ::φauto_str<T>::p;
  using τ::φlit<T>::p;
  using σ::pre::at_parsers<T>::p;
  using σ::pre::at_ct_parsers<T>::p;
  using σ::pre::at_cb_parsers<T>::p;
  using σ::pre::mux_parsers<T>::p;

  Tx auto p(τ::Γa<X> *x) const { return g.p(x); }
  Tx auto p(τ::Γ     *x) const { return g.p(x); }
};


struct Γφ : public τ::Γφ_<Γφ, πφ__<Γφ>>,
            τ::φauto_str<Γφ>,
            τ::φlit<Γφ>,
            τ::φbrack<Γφ>,
            σ::pre::at_parsers<Γφ>,
            σ::pre::at_ct_parsers<Γφ>,
            σ::pre::at_cb_parsers<Γφ>,
            σ::pre::mux_parsers<Γφ>
{
  using τ::Γφ_<Γφ, πφ__<Γφ>>::p;
  using τ::φauto_str<Γφ>::p;
  using τ::φlit<Γφ>::p;
  using τ::φbrack<Γφ>::p;
  using σ::pre::at_parsers<Γφ>::p;
  using σ::pre::at_ct_parsers<Γφ>::p;
  using σ::pre::at_cb_parsers<Γφ>::p;
  using σ::pre::mux_parsers<Γφ>::p;
};


typedef πφ__<Γφ> πφ;


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
