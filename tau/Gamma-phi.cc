#include "Gamma-phi.hh"
#include "pi-phi.hh"
#include "begin.hh"

namespace τ
{


φ<St> Γφws() { return πφws(); }
φ<St> Γφlc() { return πφlc(); }
φ<St> Γφig() { return πφig(); }

φ<St> Γφlb() { slet r = Γφwrap(φl("[")); return r; }
φ<St> Γφrb() { slet r = Γφwrap(φl("]")); return r; }


}

#include "end.hh"
