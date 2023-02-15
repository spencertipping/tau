#ifndef τπ_h
#define τπ_h


#include "eta.hh"
#include "xiio.hh"
#include "phi.hh"
#include "gamma.hh"

#include "pi-int.hh"
#include "pi-phi.hh"
#include "pi-fn.hh"


#include "begin.hh"

namespace τ
{


γ γπ(πfn const&, St = "", F<bool(πi&, πinsn const&)> = [](πi&, πinsn const&) { return true; });


}

#include "end.hh"


#endif
