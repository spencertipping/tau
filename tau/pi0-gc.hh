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
  static_assert(ghm | gam == Sc<π0r>(-1));
  static_assert(ghm & gam == 0);

  π0hg         gs[gn];
  ΣΘΔ          gΘ;
  S<π0hv<Gb>*> vs;

  void gc  (uN g, uN s);  // gc a specific generation downwards
  π0r  move(π0r) const;   // return new address during gc

  template<O9 T>
  π0r operator<<(T const &x)
    {
    }
};


template<uN Gb>
struct π0hv  // heap view: a part of the root set
{
  π0h<Gb> &h;
  virtual ~π0hv() { h.vs.erase(this); }
  virtual void mark() = 0;
  virtual void move() = 0;
};


template<uN Gb> void π0h<Gb>::gc(uN g, uN s)
{
  TODO("gc");
}


}

#include "end.hh"


#endif
