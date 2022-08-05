#ifndef tau_utf9_numerics_h
#define tau_utf9_numerics_h


#include "../types.hh"
#include "../util/numerics.hh"

#include "error-proto.hh"


#include "../module/begin.hh"

namespace tau::utf9
{

using namespace tau::util::numerics;


inline u8  cou8 (u64 x) { return ou8(x)  ? throw_internal_error<u8>("u8o")   : x; }
inline u16 cou16(u64 x) { return ou16(x) ? throw_internal_error<u16>("u16o") : x; }
inline u32 cou32(u64 x) { return ou32(x) ? throw_internal_error<u32>("u32o") : x; }

inline i8  coi8 (i64 x) { return oi8(x)  ? throw_internal_error<i8>("i8o")   : x; }
inline i16 coi16(i64 x) { return oi16(x) ? throw_internal_error<i16>("i16o") : x; }
inline i32 coi32(i64 x) { return oi32(x) ? throw_internal_error<i32>("i32o") : x; }


}

#include "../module/end.hh"


#endif
