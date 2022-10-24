#ifndef τπ0_gc_markset_h
#define τπ0_gc_markset_h


#include <algorithm>
#include <cstring>


#include "debug.hh"
#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"
#include "pi0-gc-heapspace.hh"

#include "begin.hh"


#if !defined(τπ0debug_gc_postcopy_verify)
# define τπ0debug_gc_postcopy_verify τdebug
#endif


namespace τ
{


struct π0ms  // mark-set for one generation
{
  π0h &h;



};


}

#include "end.hh"


#endif
