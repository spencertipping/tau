#ifndef tau_flux_ζb_h
#define tau_flux_ζb_h


#include <algorithm>
#include <memory>


#include "debug.hh"
#include "types.hh"
#include "numerics.hh"


#include "begin.hh"

namespace tau
{


struct ζb
{
  uNc  c;             // invariant: c is a power of 2
  u8  *xs = nullptr;  // invariant: xs[0..c-1] are allocated
  uN   ri = 0;
  uN   wi = 0;
  uN   ci = 0;

  ζb(uf8 b_) : c(1ull << b_) { xs = Rc<u8*>(std::calloc(c, 1)); }
  ~ζb()                      { std::free(xs); }

  u8  *operator+ (uN a) const { return xs + bp(a); }
  bool operator[](uN a) const { return wr() ? a < wi || a >= ri && a < ci
                                            : a >= ri && a < wi;}

  bool wr()     const { return ri > wi; }
  uN   bp(uN a) const { return wr() && a >= ci ? a - ci : a; }

  // FIXME: bounds check logic may be wrong here
  bool bc(uN a) const { return wr() ? a >= ci ? a < wi : a >= ri : a >= ri && a < wi; }
  uN   ra()     const { return wr() ? wi + (ci - ri) : wi - ri; }
  uN   wa()     const { return wr() ? ri - wi        : std::max(c - wi, ri); }

  void free(uN a)
    { if (!wr()) assert(a <= wi && a >= ri);
      else     { assert(a <= wi || a >= ri && a <= ci);
                 if (a == ci) a = 0; }
      ri = a; }

  uN alloc(uN s)
    { if      (s > wa())                                                return -1;
      else if (!wr() && s + wi > c) {    ci = wi; wi  = s; ri = bp(ri); return  0; }
      else                          { let a = wi; wi += s;              return  a; } }
};


#if tau_debug_iostream
O &operator<<(O &s, ζb const &b)
{
  return s << "ζb[c=" << b.c << " ri=" << b.ri
           << " wi=" << b.wi << " ci=" << b.ci
           << " ra=" << b.ra() << " wa=" << b.wa() << "]";
}
#endif


}

#include "end.hh"


#endif
