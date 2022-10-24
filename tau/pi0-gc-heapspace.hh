#ifndef τπ0_gc_heapspace_h
#define τπ0_gc_heapspace_h


#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"

#include "begin.hh"

namespace τ
{


struct π0hs  // a single GC heap-space (generation)
{
  B h;
  π0hs(uN s) { h.reserve(s); }

  template<O9 T> π0r operator<<(T const &x)
    { return h.size() + x.size() > h.capacity()
           ? nullptr
           : h.data() + (h << x); }
};


}

#include "end.hh"


#endif
