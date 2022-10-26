#ifndef τπ0_gc_heapspace_h
#define τπ0_gc_heapspace_h


#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"

#include "begin.hh"

namespace τ
{


// FIXME: π₀hs should be more persistent than it is; we should use B::swap()
// and have heapspaces allocate new bytestrings. Then heapspaces can track
// their own capacity set points, and we can remove some indirection in π₀h.

struct π0hs  // a single GC heap-space (generation)
{
  B h;
  π0hs(uN s) { h.reserve(s); }

  bool contains(π0r x) { return x >= h.data() && x < h.data() + h.size(); }

  template<O9 T> π0r operator<<(T const &x)
    { return h.size() + x.size() > h.capacity()
           ? nullptr
           : h.data() + (h << x); }
};


}

#include "end.hh"


#endif
