#ifndef σtoplevel_h
#define σtoplevel_h


#include "../tau.hh"
#include "lens.hh"

#include "begin.hh"

namespace σ
{


template<class T>
using σφd = τ::φd_<T>;


σφd<τ::πfn> &σφp();
σφd<σlens>  &σφl();
σφd<τ::Γ>   &σφΓ();
σφd<τ::Γp>  &σφΓp();


}

#include "end.hh"


#endif
