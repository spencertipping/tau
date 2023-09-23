#ifndef τζ_h
#define τζ_h


#include "types.hh"
#include "begin.hh"

namespace τ
{


/*

  ζ circular buffer state model

  normal state, ci == 0:
  |      ri                  wi          c
  | -----|===================|-----------|
  |                          |-----------|  ← space for next object
  |      |<----------------->|              ← ra
  |<---->|                   |<--------->|  ← wa

  wrapped state, ci != 0:
  |      wi            ri           ci   c
  | =====|-------------|============|----|
  |      |-------------|                    ← space for next object
  |                                 |xxxx|  ← lost space (this cycle)
  | |<-->|             |<---------->|       ← ra
  |      |<----------->|                    ← wa

*/


// Memory profiling for ζ
void ζtrack_alloc(uN);
void ζtrack_free(uN);
uN   ζallocated();


struct ζ
{
  ζ(uN c_) : c(c_), xs(new u8[c]), ri(0), wi(0), ci(0) { ζtrack_alloc(c); }
  ~ζ() { ζtrack_free(c); delete[] xs; }

  // Accessors relative to read point
  u8 *operator+ (uN i) const { let u = wrapped() ? ci : wi; A(ri + i <= u, "ζ+ OOB: " << ri << "+" << i << " > " << u); return xs + ri + i; }
  u8  operator[](uN i) const { let u = wrapped() ? ci : wi; A(ri + i < u, "ζ[] OOB: " << ri << "+" << i << " ≥ " << u); return xs[ri + i]; }

  uN   capacity() const { return c; }
  bool wrapped()  const { return ci; }
  uN   wrap(uN i) const { return wrapped() && i >= ci ? i - ci : i; }

  // read and write availability
  uN ra() const
    { return wrapped() ? ci - ri + wi          // (ri..ci) + (0..wi)
           :             wi - ri; }            // (ri..wi)

  uN wa() const
    { return wrapped() ? ri - wi               // (wi..ri)
           : ri        ? std::max(c - wi, ri)  // (wi..c) or (0..ri)
           :             c - wi; }             // (wi..c)


  void clear() { ri = wi = ci = 0; }

  void free(uN l)  // free l bytes of memory from the left
    { A(l <= ra(), "ζ::free(" << l << "), ra=" << ra());
      if (wrapped()) { let a = std::min(l, ci - ri); set_ri(ri + a); l -= a; }
      if (l) set_ri(ri + l); }

  // NOTE: iptr() **does not update** the write head; for that, use advance()
  u8 *iptr(uN l)   // insertion pointer to l bytes, or null
    { if      (l > wa())                                                   return nullptr;
      else if (!wrapped() && l + wi > c) { ci = wi; wi = 0; ri = wrap(ri); return xs; }
      else                                                                 return xs + wi; }

  void advance(uN l)  // advance writer by l bytes; you must have called iptr(l) first
    { wi += l; }


  ζ &resize(uN);
  ζ &ensure(uN c_) { return c_ > c ? resize(c_) : *this; }


protected:
  uN  c;   // capacity in bytes
  u8 *xs;  // circular buffer
  uN  ri;  // read index (0..c-1)
  uN  wi;  // write index (0..c-1)
  uN  ci;  // circular index (0..c-1)

  friend O &operator<<(O&, ζ const&);

  // Auto-unwrap if ri hits the upper boundary, auto-defragment
  // if all data is consumed (defrag just means ri = wi = 0)
  void set_ri(uN x)
    { if ((ri = x) == ci)         ri = ci = 0;
      if (!wrapped() && ri == wi) clear(); }
};


O &operator<<(O &s, ζ const &z);


}

#include "end.hh"


#endif
