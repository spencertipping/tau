#ifndef τΓφbasic_h
#define τΓφbasic_h

#include "phi.hh"
#include "phi-ctor.hh"
#include "pi-phi-basic.hh"
#include "Gamma.hh"
#include "Psi.hh"

#include "begin.hh"

namespace τ
{


struct    Γident  { St x; };
Tt struct Γrbrack { T x; };


// Built-in language elements
φ<St> Γφws();
φ<St> Γφlc();
φ<St> Γφig();

φ<St> Γφlb();
φ<St> Γφrb();

Tt φ<T> Γφwrap (φ<T> p) { return φ2("wrap", Γφig(), p, Γφig()); }
Tt φ<T> Γφgroup(φ<T> p) { return φ2("[]", Γφlb(), Γφwrap(p), Γφrb()); }


}

#include "end.hh"

#endif
