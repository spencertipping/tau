#ifndef τπ0_gc_heap_h
#define τπ0_gc_heap_h


#include "types.hh"
#include "Sigma.hh"
#include "pi0-types.hh"
#include "pi0-gc-heapspace.hh"

#include "pi0-begin.hh"

namespace τ
{


π0TGs π0h   // a multi-generational heap (Gb = generation bits)
{
  π0TS;
  sletc gn = 1u << Gb;

  uNc           is;      // auto-inlining size
  uNc           s0;      // newgen size
  uNc           sb;      // expansion bits per generation
  π0T(π0hs)    *hs[gn];  // generations; 0 = newest
  π0T(π0ms)    *ms[gn];  // during GC, mark-set for each gen
  π0T(π0gs)    *gs[gn];  // during GC, splice map for each gen
  ΣΘΔ           gΘ;
  M<π0R, π0R>  *rm;      // reference moves
  S<π0T(π0hv)*> vs;      // views that comprise the root set

  π0h(uN is_ = 64, uN s0_ = 65536, uN sb_ = 0)
  : is(is_), s0(s0_), sb(sb_), rm(nullptr)
  { for (uN g = 0; g < gn; ++g)
    { ms[g] = nullptr;
      gs[g] = nullptr;
      hs[g] = new π0T(π0hs)(g, s0 << g * sb); } }

  ~π0h();


  i9 operator[](π0R x) { return (*hs[x.g()])[x.a()]; }

  π0R operator()(ζp x) const
    { for (π0hg i = 0; i < gn; ++i)
      { let y = (*this)(i, x); if (!y.ω()) return y; }
      return π0R(); }

  π0R operator()(π0hg g, ζp x) const
    { let &b = hs[g]->h;

      // NOTE: upper bound is included to provide endpoint/next
      // continuity, even though it isn't a valid reference.
      //
      // FIXME: this may break everything if heap-spaces are allocated
      // immediately adjacent to one another; we probably need to size
      // them up by one to prevent this.
      return x >= b.data() && x <= b.data() + b.size()
           ? π0R(g, x - b.data())
           : π0R(); }


  // TODO: we need a "flatten all references" method so we can compare
  // values and send them across Φ boundary


  π0TO π0R operator<<(π0To const &x)
    { let a = *hs[0] << x;
      if (!a.ω()) return a;
      gc(x.size());
      return *hs[0] << x; }


  void gc  (uN s = 0);   // GC to allocate s bytes of space
  void mark(π0R);        // externally mark a reference
  π0R  move(π0R) const;  // used by π0hv to translate old → new ext refs
};


}

#include "pi0-end.hh"


#endif
