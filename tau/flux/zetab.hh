#ifndef tau_flux_zetab_h
#define tau_flux_zetab_h


#include <memory>


#include "../types.hh"
#include "../numerics.hh"


#include "../module/begin.hh"

namespace tau::flux
{


/*
ζb: a circular byte buffer with entry-level ref counting

Entries are allocated inline and have refcount and length prefixes sized
by T (if required).

TODO: should we implement compaction instead of wasting space?
*/

template<class T, class C = u8>
struct ζb
{
  uNc  c;
  u8  *xs = nullptr;
  u64  ri = 0;  // read index
  u64  wi = 0;  // write index
  uN   oi = 0;  // offset index (moonotonic increase)

  ζb(uN c_) : c(c_) { xs = Rc<u8*>(std::calloc(c, 1)); }
  ~ζb()             { std::free(xs); }

  // TODO: differentiate between xs-absolute and xs+oi relative
  uN   size()   const { return wi - ri; }
  C    rc(uN i) const { return R<C>(xs, i); }
  void rc(uN i, C x)  { W<C>(xs, i, x); }

  uN start(uN i) const { return i + sizeof(C); }
  uN len  (uN i) const { return T::len(xs, start(i), c); }
  uN end  (uN i) const { return i + len(i); }

  ζb<T, C> &free(uN i)
    { let r = rc(i);
      assert(r);
      rc(i, r - 1);
      if (r - 1 == 0) ff_ri();
      return *this; }

  uN alloc(uN l)
    { let x = wi;
      W<C>(xs, wi++, 1);
      // TODO: check capacity and wrapping
    }


  void ff_ri()
    { while (ri < wi && !rc(ri)) ri = end(ri);
      // TODO: advance oi
    }
};


}

#include "../module/end.hh"


#endif
