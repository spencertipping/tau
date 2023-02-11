#ifndef τπ_h
#define τπ_h


#include "eta.hh"
#include "xiio.hh"
#include "phi.hh"
#include "gamma.hh"

#include "begin.hh"

namespace τ
{


typedef Sp<void> π;  // TODO


// Parser for all τ-defined γs
φ<γ> φγ();


// Parser for π η-transformers
φ<π> φπ();


}

#include "end.hh"


#endif
