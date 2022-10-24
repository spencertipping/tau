#ifndef τπ0_gc_heapspace_h
#define τπ0_gc_heapspace_h


#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"

#include "pi0-begin.hh"

namespace τ
{


π0TGs π0hs  // a single GC heap-space (generation)
{
  π0TS;
  π0hg const g;
  B          h;

  // TODO: add finalizer functions

  π0hs(π0hg g_, uN s) : g(g_) { h.reserve(s); }

  π0TO π0R operator<<(π0To const &x)
    { return h.size() + x.size() > h.capacity() - 1
           ? π0R()
           : π0R(g, h << x); }

  i9 operator[](uN x) { return i9{h.data() + x}; }
  i9 operator[](π0R x)
  { A(x.g() == g, "π₀hs: gen " << g << " ≠ " << x.g());
    return (*this)[x.a()]; }
};


#if τdebug_iostream
π0TG O &operator<<(O &s, π0T(π0hs) const &h)
{
  s << "π₀hg " << h.g << std::endl;
  uN i = 0;
  for (let c : h.h)
  { s << "0123456789abcdef"[c >> 4]
      << "0123456789abcdef"[c & 15];
    if (!(++i & 3))    s << " ";
    if (!(  i & 31))   s << std::endl;
    if (!(  i & 1023)) s << std::endl; }
  return s;
}
#endif


}

#include "pi0-end.hh"


#endif
