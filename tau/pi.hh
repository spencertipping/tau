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


// Parser for all τ-defined γs
φ<πfn>    φγ();
φd_<πfn> &φγd();


// Parser for π η-transformers
φ<πfn>    φπ();
φa_<πfn> &φπa();
φd_<πfn> &φπd();


}

#include "end.hh"


#endif
