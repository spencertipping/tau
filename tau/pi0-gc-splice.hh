#ifndef τπ0_gc_splice_h
#define τπ0_gc_splice_h


#include "types.hh"
#include "pi0-types.hh"

#include "pi0-gc-begin.hh"

namespace τ
{


π0TGs π0gs  // splice map for a single heap generation
{ π0TS;
  B           &h;
  uNc          g;
  S<π0T(π0r)>  ri;  // references to be inlined

  // planned size of old-space ref (once we copy it)
  uN planned_size(π0T(π0r) x)
    { TODO("planned_size");
    }

  void plan_inline(π0T(π0r) x)
    { ri.insert(x);
    }
};


}

#include "pi0-gc-end.hh"


#endif
