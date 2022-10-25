#ifndef τπ0gc_o9_h
#define τπ0gc_o9_h


#include "utf9.hh"
#include "types.hh"
#include "pi0-types.hh"
#include "pi0-gc-markset.hh"


#if !defined(τπ0debug_gc_postcopy_verify)
# define τπ0debug_gc_postcopy_verify τdebug
#endif


#include "begin.hh"

namespace τ
{


// NOTE: π₀ho9 is used during GC to inline/move things, and to flatten
// objects to contextualize them for Φ. In the latter case, h is null.

struct π0ho9 : virtual o9V
{
  π0r             r;       // object being written
  π0h            *h;       // heap, if used during GC
  uN mutable      is = 0;  // post-rewrite size
  V<P<uN, π0ho9>> f;       // flagged children

  π0ho9(π0r r_, π0h *h_) : r(r_), h(h_) {}

  uN isize() const
    { let i = i9{r};
      if (is)           return is;
      if (!i.flagged()) return is = i.size();
      if (u9coll[i.type()])
      {

      }

      TODO("isize() flagged");

      return is; }

  uN size()      const { return isize() + u9sb(u9sq(isize())); }
  uN write(ζp m) const
    { if (h && h->ms) (h->ms->n)[r] = m;

      // FIXME: using o9i9{} will conflict with size if the size
      // isn't optimally packed; we need to write header + copy here
      if (!i9{r}.flagged()) return o9i9{r}.write(m);

      TODO("write() flagged");
    }
};


}

#include "end.hh"


#endif
