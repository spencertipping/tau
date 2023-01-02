#ifndef τπ0_gc_heap_h
#define τπ0_gc_heap_h


#include "types.hh"
#include "Sigma.hh"
#include "shd.hh"
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
  uN          lss0;       // last live-set size
  uN          Σa;         // total allocations
  uN          an;         // allocation count (since last GC)
  Σι          gl;         // live-set size

  π0h(uN is_ = 64, Ar<uN, gn> const &s_ = {65536, 1048576})
    : is(is_), s(s_), ms(nullptr), lss0(0), Σa(0), an(0)
    { for (uN g = 0; g < gn; ++g) hs[g] = new π0hs(s[g]); }

  ~π0h();


  template<O9 T> π0r gcw(uN, T const&);

  template<O9 T> π0r operator<<(T const &x)
    { let s = x.size();
      ++an;
      Σa += s;
      if (let a = *hs[0] << x) return a;
      return gcw(s, x); }


  uN gen(π0r x) const
    { for (uN i = 0; i < gn; ++i) if (hs[i]->contains(x)) return i;
      return gω; }


  // Make sure we can allocate s bytes without GC
  void res(uN s) { if (s > hs[0]->avail()) gc(s); }


  // Follow references until we hit something else
  i9 operator()(i9 x) const { while (x.is_πref()) x = *x; return x; }


  // Profiling method to track live-set size over time
  uN note_lss(uN lss) { gl << (lss0 = lss); return lss; }


  void   gc   (uN s = 0);   // GC to allocate s extra bytes of space
  void   mark (π0r);        // externally mark a reference
  π0r    move (π0r) const;  // used by π0hv to translate old → new ext refs
  π0ho9 *claim(π0r, π0r);   // used by π₀ho9 to mark objects as inlined

  Mo<π0r, π0ho9*>::const_iterator cb(π0r) const;
  Mo<π0r, π0ho9*>::const_iterator ce()    const;
};


}

#include "end.hh"


#endif
