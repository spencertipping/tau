#ifndef τπint_h
#define τπint_h


#include "pi-val.hh"
#include "xiio.hh"
#include "tau.hh"

#include "begin.hh"

namespace τ
{


struct πi
{
  ξi        i;
  ξo        o;
  M<St, πv> vs;

  πi(τe &t)        : o(t.l(), {}) {}
  πi(ξi i_, ξo o_) : i(i_), o(o_) {}
};


}

#include "end.hh"


#endif
