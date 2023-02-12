#ifndef τπ_h
#define τπ_h


#include "eta.hh"
#include "xiio.hh"
#include "phi.hh"
#include "gamma.hh"
#include "pi-int.hh"
#include "pi-phi.hh"

#include "begin.hh"

namespace τ
{


typedef πf<η0o&&> πηf;
typedef πf<γ>     πγf;


// Parser for all τ-defined γs
φ<πγf> φγ();


// Parser for π η-transformers
φ<πηf> φπ();


}

#include "end.hh"


#endif
