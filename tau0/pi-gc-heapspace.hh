#ifndef τπ_gc_heapspace_h
#define τπ_gc_heapspace_h


#include "types.hh"
#include "utf9.hh"
#include "pi-types.hh"

#include "begin.hh"

namespace τ
{


// TODO: π₀hs should be more persistent than it is; we should use B::swap()
// and have heapspaces allocate new bytestrings. Then heapspaces can track
// their own capacity set points, and we can remove some indirection in π₀h.

struct πhs  // a single GC heap-space (generation)
{
  B h;
  πhs(uN s) { h.reserve(s); }

  bool contains(πr x) const
    { return x >= h.data() && x < h.data() + h.size(); }

  template<O9 T> πr operator<<(T const &x)
    { return h.size() + x.size() > h.capacity()
           ? nullptr
           : h.data() + (h << x); }

  uN avail() const { return h.capacity() - h.size(); }
};


}

#include "end.hh"


#endif
