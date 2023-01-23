#ifndef τη0o_h
#define τη0o_h


#include <string>

#include <zstd.h>

#include "../dep/picosha3.h"


#include "types.hh"
#include "eta0i.hh"
#include "begin.hh"

namespace τ
{


// η₀ structure output stream: incrementally builds an η₀ frame
struct η0o
{
  η0o()        : h({0}), hs(0) {}
  η0o(η0o &&x) : h(x.h), hs(x.hs), xs(std::move(x.xs)) {}

  η0o operator=(η0o &&x)
    { h  = x.h;
      hs = x.hs;
      xs = std::move(x.xs); }

protected:
  Ar<u8, 10> h;
  u8         hs;
  V<St>      xs;
};


}


#include "end.hh"


#endif
