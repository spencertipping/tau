#ifndef tau_flux_zetab_h
#define tau_flux_zetab_h


#include <memory>


#include "../types.hh"
#include "../numerics.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct ζb
{
  uNc  c;             // invariant: power of two
  u8  *xs = nullptr;  // invariant: xs[0..c-1] are valid
  u64  ri = 0;        // read index
  u64  wi = 0;        // write index
  uN   oi = 0;        // address adjustment (subtracted to index into xs)

  ζb(uN c_) : c(c_) { assert((c & c - 1) == 0); xs = Rc<u8*>(std::calloc(c, 1)); }
  ~ζb()             { std::free(xs); }

  u8 &operator[](uN a) { return xs[a - oi]; }

  // TODO: grab/drop/advance
};


}

#include "../module/end.hh"


#endif
