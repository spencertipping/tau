#ifndef σauto_h
#define σauto_h

#include "begin.hh"


// Definitions for parser auto-fills
namespace σ
{

τ::φ<τ::πf> &φπ();
τ::φ<τ::Γ>  &φΓa();
τ::φ<τ::Ψ0> &φΨ0();
τ::φ<τ::Ψ1> &φΨ1();
τ::φ<τ::Ψ2> &φΨ2();
τ::φ<τ::Ψ4> &φΨ4();

}


namespace τ
{

template<> struct φauto_<i16> { static φ<i16> p() { return φm(πφwrap(πφint()), [](i64 x) { return Sc<i16>(x); }); } };
template<> struct φauto_<i32> { static φ<i32> p() { return φm(πφwrap(πφint()), [](i64 x) { return Sc<i32>(x); }); } };
template<> struct φauto_<i64> { static φ<i64> p() { return πφwrap(πφint());   } };
template<> struct φauto_<f64> { static φ<f64> p() { return πφwrap(πφfloat()); } };
template<> struct φauto_<St>  { static φ<St>  p() { return πφwrap(πφstr());   } };
template<> struct φauto_<πv>  { static φ<πv>  p() { return πφwrap(πφlit());   } };

template<> struct φauto_<πf>  { static φ<πf>  p() { return σ::φπ();   } };
template<> struct φauto_<Γ>   { static φ<Γ>   p() { return σ::φΓa();  } };
template<> struct φauto_<Ψ0>  { static φ<Ψ0>  p() { return σ::φΨ0();  } };
template<> struct φauto_<Ψ1>  { static φ<Ψ1>  p() { return σ::φΨ1();  } };
template<> struct φauto_<Ψ2>  { static φ<Ψ2>  p() { return σ::φΨ2();  } };
template<> struct φauto_<Ψ4>  { static φ<Ψ4>  p() { return σ::φΨ4();  } };

}

#include "end.hh"

#endif
