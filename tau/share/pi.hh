#ifndef τshare_π_h
#define τshare_π_h


#include "../gamma.hh"
#include "../pi.hh"

#include "topology.hh"

#include "../begin.hh"

namespace τ
{


// Run a π program, optionally with a filter
γ γπ(πfn const&, St = "", F<bool(πi&, πinsn const&)> = [](πi&, πinsn const&) { return true; });


}

#include "../end.hh"


#endif
