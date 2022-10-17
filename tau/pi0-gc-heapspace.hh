#ifndef τπ0_gc_heapspace_h
#define τπ0_gc_heapspace_h


#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"

#include "pi0-gc-begin.hh"

namespace τ
{


π0TGs π0hs  // a single GC heap-space (generation)
{
  π0TS;
  π0hg const g;
  B          h;

  π0hs(π0hg g_, uN s) : g(g_) { h.reserve(s); }

  π0TO π0R operator<<(π0To const &x)
    { return h.size() + x.size() > h.capacity()
           ? π0R()
           : π0R(g, h << x); }

  i9 operator[](uN x) { return i9{h.data() + x}; }
  i9 operator[](π0R x)
  { A(x.g() == g, "π₀hs: gen " << g << " ≠ " << x.g());
    return (*this)[x.a()]; }
};


}

#include "pi0-gc-end.hh"


#endif
