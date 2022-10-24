#ifndef τπ0_gc_markset_h
#define τπ0_gc_markset_h


#include <algorithm>
#include <cstring>


#include "debug.hh"
#include "types.hh"
#include "utf9-types.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"
#include "pi0-gc-heapspace.hh"

#include "begin.hh"


#if !defined(τπ0debug_gc_postcopy_verify)
# define τπ0debug_gc_postcopy_verify τdebug
#endif


namespace τ
{


// NOTE: π0ms and π0gs (defined below) are split deliberately to reflect
// the state-wall between them. Splicing and inlining require us to
// analyze the whole heap at once and look at objects in their allocation
// order. So we don't have anything like incremental marking.


#if τdebug_iostream
π0TGs π0gS;
π0TG O &operator<<(O &s, π0T(π0gS) const &x);
#endif


struct π0ms  // mark-set for one generation
{
  π0h     &h;
  So<π0r>  m;  // marked refs (internal + external)

  // NOTE: no need to chase down references-to-references because we will
  // always inline small objects (like references)
  void mi(π0r x, π0r y)  // mark internal reference x → y
  { me(y);
    h.mark(y); }         // important: mark into other generations

  void me(π0r x)         // mark external reference to x
  { if (x.g() == g && !m.contains(x))
    { m.insert(x);
      for (let i : h[x].flags())
        // NOTE: we track references in all generations in case older
        // generations end up being collected here -- we just don't
        // trace those older-gen objects yet
        if (i.is_πref()) mi(x, π0R(i)); } }
};


}

#include "end.hh"


#endif
