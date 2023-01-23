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
  η0o() {}

protected:
  η0t  t;  // intended type
  u8   c;  // compression level; 0 = no compression
  bool h;  // if true, add a hash
  union { void *p; u64 u; i64 i; f64 d; f32 f; St s; bool b; };
};


}


#include "end.hh"


#endif
