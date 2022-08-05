#ifndef tau_utf9_hash_h
#define tau_utf9_hash_h


#define XXH_INLINE_ALL 1
# include "../deps/xxhash.h"
# include "../deps/picosha2.h"
#undef XXH_INLINE_ALL


#include "../types.hh"

#include "numerics.hh"


#include "../module/begin.hh"

namespace tau::utf9
{


static_assert(sizeof(XXH64_hash_t) == sizeof(u64));

inline u64 xxh(void const *x, uN l, u64 s) { return XXH64(x, l, s); }
inline u64 xxc(u64 a, u64 b)               { a = ce(a); return XXH64(&a, sizeof a, b); }


}

#include "../module/end.hh"


#endif
