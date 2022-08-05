#ifndef tau_utf9_numerics_h
#define tau_utf9_numerics_h


#include "../util/numerics.hh"
#include "error-proto.hh"


#include "../module/begin.hh"

namespace tau::utf9
{

using namespace tau::util::numerics;


inline uint8_t  cou8 (uint64_t x) { return ou8(x)  ? throw_internal_error<uint8_t>("u8o") : x; }
inline uint16_t cou16(uint64_t x) { return ou16(x) ? throw_internal_error<uint16_t>("u16o") : x; }
inline uint32_t cou32(uint64_t x) { return ou32(x) ? throw_internal_error<uint32_t>("u32o") : x; }

inline int8_t   coi8 (int64_t x) { return oi8(x)  ? throw_internal_error<int8_t>("i8o") : x; }
inline int16_t  coi16(int64_t x) { return oi16(x) ? throw_internal_error<int16_t>("i16o") : x; }
inline int32_t  coi32(int64_t x) { return oi32(x) ? throw_internal_error<int32_t>("i32o") : x; }


}

#include "../module/end.hh"


#endif
