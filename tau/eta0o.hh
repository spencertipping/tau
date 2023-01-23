#ifndef τη0o_h
#define τη0o_h


#include <string>

#include <zstd.h>

#include "../dep/picosha3.h"


#include "types.hh"
#include "numerics.hh"
#include "eta-types.hh"
#include "eta0i.hh"
#include "begin.hh"

namespace τ
{


// η₀ structure output stream: incrementally builds an η₀ frame
struct η0o
{
  η0o()        : h({0}), hs(0) {}
  η0o(η0o &&x) : h(x.h), hs(x.hs), xs(std::move(x.xs)) {}


  uN size() const
    { uN t = hs;
      for (let &x : xs) t += x.size();
      return t; }

  void into(u8 *d) const
    { uN o = hs;
      memcpy(d, h.data(), hs);
      for (let &x : xs)
      { memcpy(d + o, x.data(), x.size());
        o += x.size(); } }

  η0o operator=(η0o &&x)
    { h  = x.h;
      hs = x.hs;
      xs = std::move(x.xs); }

  template<class T>
  std::enable_if_t<η0nt<T>::t, η0o>
  &operator=(T x)
    { let b = ilog(sizeof(x));
      h[0] = Sc<u8>(η0nt<T>::t) << 2 | b;
      W(h, 1, x);
      hs = 1 + (1 << b);
      xs.clear();
      return *this; }


  // TODO: fix up the canonical state; we should store something more like
  // η0i's variables, but with the string vector

protected:
  Ar<u8, 10> h;
  u8         hs;
  V<St>      xs;
};


}


#include "end.hh"


#endif
