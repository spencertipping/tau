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
  i9          r;        // old-space thing we're copying from
  π0h        *h;        // null if we're flattening objects for export
  uN mutable  isi = 0;  // isi = isize() << 1 | has_uninlined_refs

  π0ho9(i9 r_, π0h *h_) : r(r_), h(h_) {}


  bool flagged() const { isize(); return isi & 1; }
  uN   size()    const { return isize() + u9sb(u9sq(isize())); }
  uN   isize()   const
    { if (isi) return isi >> 1;



      //

    }



  uN write(ζp m) const
    { let i = u9ws(m, 0, r.type(), isize(), flagged());

    }
};


}

#include "end.hh"


#endif
