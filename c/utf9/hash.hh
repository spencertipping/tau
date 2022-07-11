#ifndef tau_utf9_hash_h
#define tau_utf9_hash_h


#define XXH_INLINE_ALL 1
#include "../xxhash.h"
#include "../picosha2.h"

#include "numerics.hh"

#include "../begin.hh"


static_assert(sizeof(XXH64_hash_t) == sizeof(uint64_t));


namespace tau::utf9
{

inline uint64_t xxh(void const * x, size_t l, uint64_t s) { return XXH64(x, l, s); }
inline uint64_t xxc(uint64_t a, uint64_t b)               { a = ce(a); return XXH64(&a, sizeof a, b); }

}


#include "../end.hh"

#endif
