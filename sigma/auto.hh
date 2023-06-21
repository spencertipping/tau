#ifndef σauto_h
#define σauto_h

#include "begin.hh"


// Definitions for parser auto-fills
namespace σ
{


}


namespace τ
{


/*
template<> struct φauto_<σ::σrbrack>
{ static φ<σ::σrbrack> p() { return φm(φ1("σ]", σ::φπp(), πφwrap(φl("]"))), [](πfs &&x) { return σ::σrbrack{mo(x)}; }); } };

template<> struct φauto_<σ::σrparen>
{ static φ<σ::σrparen> p() { return φm(φ1("σ)", σ::φπp(), πφwrap(φl(")"))), [](πfs &&x) { return σ::σrparen{mo(x)}; }); } };

template<> struct φauto_<σ::σrbrace>
{ static φ<σ::σrbrace> p() { return φm(φ1("σ}", σ::φπp(), πφwrap(φl("}"))), [](πfs &&x) { return σ::σrbrace{mo(x)}; }); } };

template<> struct φauto_<πf>  { static φ<πf> p() { return φL<πf>(σ::φπa); } };
*/

template<> struct φauto_<Γ>   { static φ<Γ>  p() { return φL<Γ> (σ::φΓa); } };

template<> struct φauto_<Ψ0>  { static φ<Ψ0> p() { return σ::φΨ0(); } };
template<> struct φauto_<Ψ1>  { static φ<Ψ1> p() { return σ::φΨ1(); } };
template<> struct φauto_<Ψ2>  { static φ<Ψ2> p() { return σ::φΨ2(); } };
template<> struct φauto_<Ψ4>  { static φ<Ψ4> p() { return σ::φΨ4(); } };

template<> struct φauto_<ηm>
{
  // TODO: define this
  static φ<ηm> p() { return φF<ηm>(); }
};

}

#include "end.hh"

#endif
