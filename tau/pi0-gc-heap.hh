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
  sletc gn = 2;           // number of generations
  sletc gω = Sc<uN>(-1);  // null generation -- off-heap memory

  uNc         is;         // auto-inlining size
  Ar<uN, gn>  s;          // size of each generation
  S<π0hv*>    vs;         // views that comprise the root set
  π0hs       *hs[gn];     // generations; 0 = newest
  π0ms       *ms;         // during GC, the mark-set tracker
  ΣΘΔ         gΘ;         // GC timer

  π0h(uN is_ = 64, Ar<uN, gn> const &s_ = {65536, 1048576})
    : is(is_), s(s_), ms(nullptr)
    { for (uN g = 0; g < gn; ++g) hs[g] = new π0hs(s[g]); }

  ~π0h();


  template<O9 T> π0r operator<<(T const &x)
    { if (let a = *hs[0] << x) return a;
      gc(x.size());
      return *hs[0] << x; }


  uN gen(π0r x) const
    { for (uN i = 0; i < gn; ++i) if (hs[i]->contains(x)) return i;
      return gω; }


  // Dereference all pointers
  i9 operator()(π0r x) const
    { while (i9{x}.is_πref()) x = *i9{x};
      return x; }


  void gc  (uN s = 0);   // GC to allocate s extra bytes of space
  void mark(π0r);        // externally mark a reference
  π0r  move(π0r) const;  // used by π0hv to translate old → new ext refs
  void move(π0r, π0r);   // declare a moved reference
};


}

#include "end.hh"


#endif
