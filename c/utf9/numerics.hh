#ifndef tau_utf9_numerics_h
#define tau_utf9_numerics_h


#include <bit>
#include <cstdint>

#include <byteswap.h>

#include "errors.hh"

#include "../begin.hh"


namespace tau::utf9
{

let LE = std::endian::native == std::endian::little;
let BE = std::endian::native == std::endian::big;


static_assert(LE || BE, "unsupported endianness");

static_assert(sizeof(long long) == sizeof(uint64_t));
static_assert(sizeof(double)    == sizeof(uint64_t));
static_assert(sizeof(float)     == sizeof(uint32_t));
static_assert(sizeof(char)      == sizeof(uint8_t));

static_assert(sizeof(void*)  <= sizeof(uint64_t));  // <=, not ==, required for emscripten


inline uint64_t ce(uint64_t x) { return LE ? bswap_64(x) : x; }
inline uint32_t ce(uint32_t x) { return LE ? bswap_32(x) : x; }
inline uint16_t ce(uint16_t x) { return LE ? bswap_16(x) : x; }

inline int64_t  ce(int64_t x)  { return LE ? bswap_64(x) : x; }
inline int32_t  ce(int32_t x)  { return LE ? bswap_32(x) : x; }
inline int16_t  ce(int16_t x)  { return LE ? bswap_16(x) : x; }

inline double ce(double const x)
{
  if (BE) return x;
  union { double xd; uint64_t xi; }; xd = x;
  xi = bswap_64(xi);
  return xd;
}

inline float ce(float const x)
{
  if (BE) return x;
  union { float xf; uint32_t xi; }; xf = x;
  xi = bswap_32(xi);
  return xf;
}


inline bool ou8 (uint64_t x) { return !!(x >> 8); }
inline bool ou16(uint64_t x) { return !!(x >> 16); }
inline bool ou32(uint64_t x) { return !!(x >> 32); }

inline bool oi8 (int64_t x) { return x << 56 >> 56 != x; }
inline bool oi16(int64_t x) { return x << 48 >> 48 != x; }
inline bool oi32(int64_t x) { return x << 32 >> 32 != x; }

inline uint8_t  cou8 (uint64_t x) { return ou8(x)  ? throw internal_error("u8o") : x; }
inline uint16_t cou16(uint64_t x) { return ou16(x) ? throw internal_error("u16o") : x; }
inline uint32_t cou32(uint64_t x) { return ou32(x) ? throw internal_error("u32o") : x; }

inline int8_t   coi8 (int64_t x) { return oi8(x)  ? throw internal_error("i8o") : x; }
inline int16_t  coi16(int64_t x) { return oi16(x) ? throw internal_error("i16o") : x; }
inline int32_t  coi32(int64_t x) { return oi32(x) ? throw internal_error("i32o") : x; }


}


#include "../end.hh"

#endif
