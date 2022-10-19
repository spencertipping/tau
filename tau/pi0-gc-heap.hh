#ifndef τπ0_gc_heap_h
#define τπ0_gc_heap_h


#include "types.hh"
#include "Sigma.hh"
#include "pi0-types.hh"
#include "pi0-gc-heapspace.hh"

#include "pi0-gc-begin.hh"

namespace τ
{


π0TGs π0h   // a multi-generational heap (Gb = generation bits)
{
  π0TS;
  sletc gn = 1u << Gb;

  π0T(π0hs)    *gs[gn];  // generations; 0 = newest
  π0T(π0ms)    *ms[gn];  // during GC, mark-set for each gen
  ΣΘΔ           gΘ;
  S<π0T(π0hv)*> vs;      // views that comprise the root set

  π0h(uN s0 = 65536, uN sb = 0)
  { for (uN g = 0; g < gn; ++g)
      gs[g] = new π0hs(g, s0 << g * sb); }

  ~π0h()
  { A(vs.empty(), "~π0h vs nonempty (will segfault on ~π0hv)");
    for (uN g = 0; g < gn; ++g) delete gs[g]; }


  i9 operator[](π0R x) { return (*gs[x.g()])[x.a()]; }

  π0R operator()(ζp x) const
    { for (uN i = 0; i < gn; ++i)
      { let y = (*this)(i, x); if (!y.ω()) return y; }
      return π0R(); }

  π0R operator()(uN g, ζp x) const
    { let &b = gs[g]->h;
      return x >= b.data() && x < b.data() + b.size()
           ? π0R(g, x - b.data())
           : π0R(); }


  π0TO π0R operator<<(π0To const &x)
    { let a = *gs[0] << x;
      if (!a.ω()) return a;
      gc(x.size());
      return *gs[0] << x; }


  void gc  (uN s);       // GC to allocate s bytes of space
  void mark(π0R);        // externally mark a reference
  π0R  move(π0R) const;  // used by π0hv to translate old → new ext refs
};


}

#include "pi0-gc-end.hh"


#endif
