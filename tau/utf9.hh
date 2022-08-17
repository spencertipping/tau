#ifndef tau_utf9_h
#define tau_utf9_h


#define tau_defines_utf9 1


#include <string>
#include <type_traits>


#include "arch.hh"
#include "debug.hh"


#include "utf9/error-proto.hh"
#include "utf9/ibuf.hh"
#include "utf9/obuf.hh"

#include "utf9/lfn.hh"
#include "utf9/pfn.hh"
#include "utf9/typecode.hh"
#include "utf9/primitive.hh"

#include "utf9/tval.hh"
#include "utf9/val.hh"
#include "utf9/valsearch.hh"
#include "utf9/enc.hh"
#include "utf9/ostream.hh"
#include "utf9/errors.hh"


#include "module/begin.hh"

namespace tau
{


static_assert(sizeof(tau::utf9::val) == 16);
static_assert(!std::is_polymorphic<tau::utf9::val >::value);
static_assert(!std::is_polymorphic<tau::utf9::tval>::value);
static_assert(!std::is_polymorphic<tau::utf9::ibuf>::value);


typedef tau::utf9::utf9_error       u9e;
typedef tau::utf9::decoding_error   u9de;
typedef tau::utf9::encoding_error   u9ee;
typedef tau::utf9::internal_error   u9ie;
typedef tau::utf9::toperation_error u9te;
typedef tau::utf9::voperation_error u9ve;


using tau::utf9::ce;


typedef tau::utf9::val  u9;
typedef tau::utf9::tval t9;
typedef tau::utf9::ibuf i9;
typedef tau::utf9::obuf o9;

typedef u9 const u9c;
typedef t9 const t9c;
typedef i9 const i9c;


using tau::utf9::u9n;
using tau::utf9::u9t;
using tau::utf9::u9a;
using tau::utf9::u9l;
using tau::utf9::u9s;
using tau::utf9::u9m;
using tau::utf9::u9y;

using tau::utf9::α;
using tau::utf9::ω;
using tau::utf9::ι;
using tau::utf9::κ;
using tau::utf9::τ;
using tau::utf9::ρ;
using tau::utf9::θ;


using tau::utf9::tu8;
using tau::utf9::tu16;
using tau::utf9::tu32;
using tau::utf9::tu64;

using tau::utf9::ti8;
using tau::utf9::ti16;
using tau::utf9::ti32;
using tau::utf9::ti64;

using tau::utf9::ttuple;
using tau::utf9::tarray;
using tau::utf9::tutf8;
using tau::utf9::tbytes;


using tau::utf9::operator<<;


}

#include "module/end.hh"


#endif
