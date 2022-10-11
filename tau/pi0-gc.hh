#ifndef τπ0_gc_h
#define τπ0_gc_h


#include "Sigma.hh"
#include "types.hh"
#include "pi0-types.hh"
#include "pi0-gc-splice.hh"

#include "begin.hh"

namespace τ
{


struct π0hg  // a single GC generation
{
  B h;
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
};


template<uN Gb>
struct π0hv  // heap view: a part of the root set
{
  π0h<Gb> &h;
  virtual ~π0hv() {}
  virtual void mark() = 0;
  virtual void move() = 0;  // TODO: define API for reference updates
};


}

#include "end.hh"


#endif
