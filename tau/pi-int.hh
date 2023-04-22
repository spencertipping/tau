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

  πi()                            {}
  πi(ξi i_, ξo o_) : i(i_), o(o_) {}

  [[noreturn]] void fail(St reason) { throw reason; }
};


}

#include "end.hh"


#endif
