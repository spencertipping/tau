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
  i9           const r;
  π0h               &h;
  π0ho9 const *const o;        // owner, if any (i.e. who is inlining us)
  uN         mutable isi = 0;  // isi = isize() << 1 | has_uninlined_refs

  π0ho9(i9 r_, π0h &h_, π0ho9 const *o_) : r(r_), h(h_), o(o_) {}


  bool flagged() const { isize(); return isi & 1; }
  uN   size()    const { return isize() + u9sb(u9sq(isize())); }
  uN   isize()   const
    { if (isi) return isi >> 1;

      // Simple case: we're an unflagged object that can't contain any
      // references, so use a direct i9 copy.
      //
      // FIXME: "are we flagged" and "can we i9 copy" are not the same
      // because the source object may start flagged and end unflagged.
      // This case is OK, but we need another indicator bit.
      if (!r.flagged() && !u9coll[r.type()])
        return (isi = r.size() << 1) >> 1;

      // If we're a collection, we must claim everything in our scope
      // even if nobody else has requested it yet, and even if we aren't
      // flagged.
      TODO("collection case");

      // If we're a reference, we have the option to become our
      // referent if it hasn't yet been claimed.
      if (r.is_πref())
      { let t = h.claim(*r, this);
        return t
             ? (isi = t->size() << 1 | t->flagged()) >> 1
             : r.size(); }

      TODO("is this all?");
    }



  uN write(ζp m) const
    { let i = u9ws(m, 0, r.type(), isize(), flagged());

    }
};


}

#include "end.hh"


#endif
