#ifndef τπ_gc_heap_h
#define τπ_gc_heap_h


#include "types.hh"
#include "Sigma.hh"
#include "shd.hh"
#include "pi-types.hh"
#include "pi-gc-heapspace.hh"

#include "begin.hh"

namespace τ
{


struct πh  // a multi-generational heap
{
  sletc gn = 2;           // number of generations
  sletc gω = Sc<uN>(-1);  // null generation -- off-heap memory

  uNc         is;         // auto-inlining size
  Ar<uN, gn>  s;          // size of each generation
  S<πhv*>     vs;         // views that comprise the root set
  πhs        *hs[gn];     // generations; 0 = newest
  πms        *ms;         // during GC, the mark-set tracker
  ΣΘΔ         gΘ;         // GC timer
  uN          lss0;       // last live-set size
  uN          Σa;         // total allocations
  uN          an;         // allocation count (since last GC)
  Σι          gl;         // live-set size

  πh(uN is_ = 64, Ar<uN, gn> const &s_ = {65536, 1048576})
    : is(is_), s(s_), ms(nullptr), lss0(0), Σa(0), an(0)
    { for (uN g = 0; g < gn; ++g) hs[g] = new πhs(s[g]); }

  ~πh();


  // Buffer a value to a malloc() area, then GC, then return the
  // buffered value once it's moved onto the heap.
  //
  // FIXME: does this even work?
  template<O9 T> πr gcw(uN, T const&);

  template<O9 T> πr operator<<(T const &x)
    { let s = x.size();
      ++an;
      Σa += s;
      if (let a = *hs[0] << x) return a;
      return gcw(s, x); }


  uN gen(πr x) const
    { for (uN i = 0; i < gn; ++i) if (hs[i]->contains(x)) return i;
      return gω; }


  // Make sure we can allocate s bytes without GC
  void res(uN s) { if (s > hs[0]->avail()) gc(s); }


  // Profiling method to track live-set size over time
  uN note_lss(uN lss) { gl << (lss0 = lss); return lss; }


  void  gc   (uN s = 0);  // GC to allocate s extra bytes of space
  void  mark (πr);        // externally mark a reference
  πr    move (πr) const;  // used by π0hv to translate old → new ext refs
  πho9 *claim(πr, πr);    // used by π₀ho9 to mark objects as inlined

  Mo<πr, πho9*>::const_iterator cb(πr) const;
  Mo<πr, πho9*>::const_iterator ce()   const;
};


}

#include "end.hh"


#endif
