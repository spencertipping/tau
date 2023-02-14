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


// Parser for all τ-defined γs
φ<πef> φγ();

template<class... Xs>
void φγdef(Xs const&... xs)
{
  dpc<φd_<πef>>(φγ())->def(xs...);
}


// Parser for π η-transformers
φ<πef> φπ();

template<class... Xs>
void φπdef(Xs const&... xs)
{
  dpc<φd_<πef>>(φπ())->def(xs...);
}


}

#include "end.hh"


#endif
