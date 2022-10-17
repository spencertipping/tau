#ifndef τπ0_gc_splice_h
#define τπ0_gc_splice_h


#include "types.hh"
#include "pi0-types.hh"

#include "begin.hh"

namespace τ
{


struct π0gs  // splice map for a single heap generation
{
  B      &h;
  S<π0r>  ri;  // references to be inlined

  void plan_inline(π0r x)  // inline a reference in old-space addresses
    {
    }

  void plan_move(π0r x, uN g) {}
};


}

#include "end.hh"


#endif
