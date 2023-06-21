#ifndef τΓφ_h
#define τΓφ_h

#include "phi.hh"
#include "phi-ctor.hh"
#include "Gamma.hh"
#include "Psi.hh"

#include "begin.hh"

namespace τ
{


// TODO: Γφ class, like πφ


// Built-in language elements
φ<St> Γφws();
φ<St> Γφlc();
φ<St> Γφig();

φ<St> Γφlb();
φ<St> Γφrb();

template<class X>
φ<X> Γφwrap(φ<X> p) { return φ2("wrap", Γφig(), p, Γφig()); }

template<class X>
φ<X> Γφgroup(φ<X> p) { return φ2("[]", Γφlb(), Γφwrap(p), Γφrb()); }


}

#include "end.hh"

#endif
