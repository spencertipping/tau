#ifndef τπ_h
#define τπ_h


#include "gamma.hh"

#include "pi-int.hh"
#include "pi-phi.hh"
#include "pi-fn.hh"
#include "pi-val.hh"


#include "begin.hh"

namespace τ
{


// Run a π program, optionally with a filter
γ γπ(πfn const&, St = "", F<bool(πi&, πinsn const&)> = [](πi&, πinsn const&) { return true; });


}

#include "end.hh"


#endif
