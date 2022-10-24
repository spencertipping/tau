#ifndef τπ0gc_o9_h
#define τπ0gc_o9_h


#include "utf9.hh"
#include "types.hh"
#include "pi0-types.hh"
#include "pi0-gc-markset.hh"

#include "begin.hh"

namespace τ
{


// NOTE: π₀ho9 is used during GC to inline/move things, and to flatten
// objects to contextualize them for Φ. In the latter case, h is null.

struct π0ho9 : virtual o9V
{
  π0r        r;       // object being written
  π0h       *h;       // heap, if used during GC
  uN mutable is = 0;  // post-rewrite size

  π0ho9(π0r r_, π0h *h_) : r(r_), h(h_) {}

  uN isize() const
    { if (is)               return is;
      if (!i9{r}.flagged()) return is = i9{r}.size();

      TODO("isize() flagged");

      return is; }

  uN size()      const { return isize() + u9sb(u9sq(isize())); }
  uN write(ζp m) const
    { if (h && h->rm)       (*h->rm)[r] = m;
      if (!i9{r}.flagged()) return o9i9{r}.write(m);

      TODO("write() flagged");
    }
};


}

#include "end.hh"


#endif
