#ifndef τπ0_gc_h
#define τπ0_gc_h


#include "Sigma.hh"
#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc-splice.hh"

#include "begin.hh"

namespace τ
{


struct π0hg  // a single GC generation
{
  B h;

  template<O9 T>
  π0r operator<<(T const &x)
    { return h.size() + x.size() > h.capacity() ? π0hω : h << x; }
};


template<uN Gb> struct π0hv;

template<uN Gb = 2>
struct π0h   // a multi-generational heap (Gb = generation bits)
{
  sletc gn   = 1 << Gb;
  sletc ghms = τwordsize - Gb;
  sletc ghm  = Sc<π0r>(gn - 1) << ghms;
  sletc gam  = ~ghm;
  static_assert(ghm | gam == π0hω);
  static_assert(ghm & gam == 0);

  static ic uN  rg (π0r x)      { return x >> ghms; }  // ref → generation
  static ic uN  ra (π0r x)      { return x  & gam; }   // ref → address
  static ic π0r gar(uN g, uN a) { return g << ghms | a; }


  π0hg         gs[gn];    // generations; 0 = newest
  ΣΘΔ          gΘ;
  bool         ga;        // is GC active
  S<π0hv<Gb>*> vs;        // views that comprise the root set

  π0h () : ga{false} {}
  ~π0h() { A(vs.empty(), "~π0h vs nonempty (will segfault on ~π0hv)"); }

  template<O9 T>
  π0r operator<<(T const &x)
    { let a = gs[0] << x;
      if (a != π0hω) return gar(0, a);
      gc(0, x.size());    // invariant: post-GC we will have space for x
      return gar(0, gs[0] << x); }


  void gc  (uN g, uN s);  // gc a specific generation downwards
  π0r  move(π0r) const;   // used by π0hv to translate old → new ext refs
  void mark(π0r) const;   // externally mark a reference
};


template<uN Gb>
struct π0hv  // heap view: a part of the root set
{
  π0h<Gb> &h;
  virtual ~π0hv() { h.vs.erase(this); }
  virtual void mark() = 0;  // mark everything in this view
  virtual void move() = 0;  // move everything in this view
};


template<uN Gb>
void π0h<Gb>::gc(uN g, uN s)
{
  TODO("gc");
}


}

#include "end.hh"


#endif
