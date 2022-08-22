#ifndef tau_flux_zetab_h
#define tau_flux_zetab_h


#include <memory>


#include "../debug.hh"
#include "../types.hh"
#include "../numerics.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct ζb
{
  uNc  c;             // invariant: power of two
  uNc  m;             // wrap mask
  u8  *xs = nullptr;  // invariant: xs[0..c-1] are valid
  uN   ri = 0;
  uN   wi = 0;
  uN   ci = 0;

  ζb(uN c_) : c(c_), m(c_ - 1) { assert((c & m) == 0); xs = Rc<u8*>(std::calloc(c, 1)); }
  ~ζb()                        { std::free(xs); }

  bool wr()     const { return ri > wi; }
  uN   bp(uN a) const { return wr() && a > ci ? a - ci : a; }
  bool bc(uN a) const { return wr() ? a > ci ? a < wi : a >= ri : a >= ri && a < wi; }

  bool lt(uN a, uN b) const
    { // TODO

    }

  // TODO: this can probably be simplified
  iN ma(uN s)
    { if (s > c)                                            return -1;
      if (wr()) { if (s + wi > ri)                          return -1;
                  else               { let a = wi; wi += s; return  a; } }
      else      { if (s + wi > c) { if (s > ri)             return -1;
                                    else { ci = wi; wi = s; return  0; } }
                  else               { let a = wi; wi += s; return  a; } } }

  ζb &fr(uN a)  // free all memory up to specified address
    {
    }
};


#if tau_debug_iostream
O &operator<<(O &s, ζb const &b)
{
  return s << "TODO: ζb";
}
#endif


}

#include "../module/end.hh"


#endif
