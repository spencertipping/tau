#ifndef τπ0gc_o9_h
#define τπ0gc_o9_h


#include "utf9.hh"
#include "types.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"

#include "begin.hh"

namespace τ
{


struct π0ho9 : virtual o9V
{
  i9          r;
  π0h        *h;  // null if we're flattening objects for export
  V<π0ho9>    c;  // child writers -- we can't store these in the heap
  uN mutable  is = 0;

  π0ho9(i9 r_, π0h *h_) : r(r_), h(h_) {}


  uN size()  const { return isize() + u9sb(u9sq(isize())); }
  uN isize() const
    { if (is) return is;

    }

  uN write(ζp m) const
    { let i = u9ws(m, 0, r.type(), isize(), !c.empty());

    }
};


}

#include "end.hh"


#endif
