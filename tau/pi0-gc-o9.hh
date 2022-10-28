#ifndef τπ0gc_o9_h
#define τπ0gc_o9_h


#include "utf9.hh"
#include "types.hh"
#include "pi0-types.hh"
#include "pi0-gc-markset.hh"

#include "begin.hh"

namespace τ
{


struct π0ho9n : virtual o9V  // non-flagged object writer
{
  i9   r;
  π0h *h;
  π0ho9n(π0r r_, π0h *h_) : r(r_), h(h_)
    { A(!r.flagged(), "π₀ho9n on flagged " << r); }
  uN size()      const { return o9i9{r}.size(); }
  uN write(ζp m) const { if (h) h->move(r, m); return o9i9{r}.write(m); }
};


// NOTE: π₀ho9f is used during GC to inline/move things, and to flatten
// objects to contextualize them for Φ. In the latter case, h is null.

struct π0ho9f : virtual o9V  // flagged-object writer
{
  i9               r;        // object being written
  π0h             *h;        // heap, if used during GC
  V<P<uN, π0ho9f>> f;        // flagged children
  uN mutable       is = 0;   // post-rewrite size

  // NOTE: this object is created on non-flagged old-space objects iff
  // they have stolen children, which will result in them being flagged
  // when written.
  π0ho9f(π0r r_, π0h *h_) : r(r_), h(h_) {}

  uN isize() const
    { if (is) return is;
      if (u9coll[r.type()])
      {

      }

      TODO("isize()");

      return is; }

  uN size()      const { return isize() + u9sb(u9sq(isize())); }
  uN write(ζp m) const
    { if (h) h->move(r, m);

      TODO("write()");
    }
};


template<> struct o9_<π0ho9n> { sletc v = true; };
template<> struct o9_<π0ho9f> { sletc v = true; };


inline o9V *o9(i9 r, π0h *h)
{
  TODO("fix for stolen children");
  return r.flagged()
       ? Sc<o9V*>(new π0ho9f{r, h})
       : Sc<o9V*>(new π0ho9n{r, h});
}


}

#include "end.hh"


#endif
