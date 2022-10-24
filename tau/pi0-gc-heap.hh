#ifndef τπ0_gc_heap_h
#define τπ0_gc_heap_h


#include "types.hh"
#include "Sigma.hh"
#include "pi0-types.hh"
#include "pi0-gc-heapspace.hh"

#include "begin.hh"

namespace τ
{


struct π0h  // a multi-generational heap
{
  sletc gn = 4;          // number of generations

  uNc           is;      // auto-inlining size
  uNc           s0;      // newgen size
  uNc           sb;      // expansion bits per generation
  π0hs         *hs[gn];  // generations; 0 = newest
  π0ms         *ms[gn];  // during GC, mark-set for each gen
  ΣΘΔ           gΘ;      // GC timer
  M<π0r, π0r>  *rm;      // reference moves
  S<π0hv*>      vs;      // views that comprise the root set

  π0h(uN is_ = 64, uN s0_ = 65536, uN sb_ = 0)
  : is(is_), s0(s0_), sb(sb_), rm(nullptr)
  { for (uN g = 0; g < gn; ++g)
    { ms[g] = nullptr;
      hs[g] = new π0hs(s0 << g * sb); } }

  ~π0h();


  template<O9 T> π0r operator<<(T const &x)
    { let a = *hs[0] << x;
      if (!a.ω()) return a;
      gc(x.size());
      return *hs[0] << x; }


  void gc  (uN s = 0);   // GC to allocate s bytes of space
  void mark(π0r);        // externally mark a reference
  π0r  move(π0r) const;  // used by π0hv to translate old → new ext refs
};


}

#include "end.hh"


#endif
