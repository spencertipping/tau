#ifndef τπ0_gc_heap_h
#define τπ0_gc_heap_h


#include "types.hh"
#include "pi0-types.hh"
#include "pi0-gc-heapspace.hh"
#include "pi0-gc-splice.hh"

#include "pi0-gc-begin.hh"

namespace τ
{


π0TGs π0h   // a multi-generational heap (Gb = generation bits)
{
  π0TS;
  sletc gn = Sc<uN>(1) << Gb;

  π0T(π0hs)    *gs[gn];    // generations; 0 = newest
  π0T(π0ms)    *ms[gn];    // during GC, mark-set for each gen
  π0T(π0gs)    *ss[gn];    // during GC, splice plan for each gen
  ΣΘΔ           gΘ;
  S<π0T(π0hv)*> vs;        // views that comprise the root set
  bool          ga;        // is GC active

  π0h(uN s0 = 65536, uN sb = 0) : ga{false}
  { for (uN g = 0; g < gn; ++g)
    { if (gs[g]) delete gs[g];
      gs[g] = new π0hs(g, s0 << g * sb); } }

  ~π0h()
  { A(vs.empty(), "~π0h vs nonempty (will segfault on ~π0hv)");
    for (uN g = 0; g < gn; ++g) delete gs[g]; }


  i9 operator[](π0T(π0r) x) { return (*gs[x.g()])[x.a()]; }

  π0T(π0r) operator()(ζp x) const
    { for (uN i = 0; i < gn; ++i)
      { let y = (*this)(i, x); if (!y.ω()) return y; }
      return π0T(π0r)(); }

  π0T(π0r) operator()(uN g, ζp x) const
    { let &b = gs[g]->h;
      return x >= b.data() && x < b.data() + b.size()
           ? π0T(π0r)(g, x - b.data())
           : π0T(π0r)(); }


  π0TO π0T(π0r) operator<<(π0To const &x)
    { let a = *gs[0] << x;
      if (!a.ω()) return a;
      gc(0, x.size());
      return *gs[0] << x; }


  void     gc  (uN g, uN s);       // gc a specific generation downwards
  π0T(π0r) move(π0T(π0r)) const;   // used by π0hv to translate old → new ext refs
  void     mark(π0T(π0r)) const;   // used by π0hv to externally mark a reference
};


}

#include "pi0-gc-end.hh"


#endif
