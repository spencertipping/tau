#ifndef tau_flux_zetab_h
#define tau_flux_zetab_h


#include <memory>


#include "../types.hh"


#include "../module/begin.hh"

namespace tau::flux
{


/*
ζb: a circular byte buffer with entry-level ref counting

Entries are allocated inline and have refcount and length prefixes sized
to the capacity. Refcounts are always one byte, lengths are the smallest
size that can encode the full capacity.
*/

struct ζb
{
  uNc  c;
  u8  *xs = nullptr;
  u64  ri = 0;  // read index
  u64  wi = 0;  // write index
  uN   oi = 0;  // offset index (monotonic increase)

  ζb(uN c_) : c(c_) { xs = Rc<u8*>(std::calloc(c, 1)); }
  ~ζb()             { std::free(xs); }

  uN size() const { return wi - ri; }
};


}

#include "../module/end.hh"


#endif
