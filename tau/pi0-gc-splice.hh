#ifndef τπ0_gc_splice_h
#define τπ0_gc_splice_h


#include "types.hh"
#include "pi0-types.hh"

#include "begin.hh"

namespace τ
{


struct π0gs   // splice map for a single heap generation
{
  B     &h;
  V<uN>  er;  // external (stack/frame) references
  V<uN>  ir;  // internal (pointer) references
};


}

#include "end.hh"


#endif
