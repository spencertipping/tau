#ifndef tau_util_numerics_h
#define tau_util_numerics_h


#include <bit>
#include <cstdint>

#include "../module/begin.hh"


namespace tau::util::numerics
{


let constexpr LE = std::endian::native == std::endian::little;
let constexpr BE = std::endian::native == std::endian::big;


static_assert(LE || BE, "unsupported endianness");

static_assert(sizeof(long long) == sizeof(uint64_t));
static_assert(sizeof(double)    == sizeof(uint64_t));
static_assert(sizeof(float)     == sizeof(uint32_t));
static_assert(sizeof(char)      == sizeof(uint8_t));

static_assert(sizeof(void*) <= sizeof(uint64_t));  // <=, not ==, required for emscripten


inline constexpr uint16_t bs16(uint16_t x) { return      x >> 8   | x                                              << 8; }
inline constexpr uint32_t bs32(uint32_t x) { return bs16(x >> 16) | static_cast<uint32_t>(bs16(x & 0xffffull))     << 16; }
inline constexpr uint64_t bs64(uint64_t x) { return bs32(x >> 32) | static_cast<uint64_t>(bs32(x & 0xffffffffull)) << 32; }


inline constexpr uint64_t ce(uint64_t x) { return LE ? bs64(x) : x; }
inline constexpr uint32_t ce(uint32_t x) { return LE ? bs32(x) : x; }
inline constexpr uint16_t ce(uint16_t x) { return LE ? bs16(x) : x; }

inline constexpr int64_t  ce(int64_t x)  { return LE ? bs64(x) : x; }
inline constexpr int32_t  ce(int32_t x)  { return LE ? bs32(x) : x; }
inline constexpr int16_t  ce(int16_t x)  { return LE ? bs16(x) : x; }

inline constexpr double ce(double const x)
{
  if (BE) return x;
  union { double xd; uint64_t xi; }; xd = x;
  xi = bs64(xi);
  return xd;
}

inline constexpr float ce(float const x)
{
  if (BE) return x;
  union { float xf; uint32_t xi; }; xf = x;
  xi = bs32(xi);
  return xf;
}


inline constexpr bool ou8 (uint64_t x) { return !!(x >> 8); }
inline constexpr bool ou16(uint64_t x) { return !!(x >> 16); }
inline constexpr bool ou32(uint64_t x) { return !!(x >> 32); }

inline constexpr bool oi8 (int64_t x) { return x << 56 >> 56 != x; }
inline constexpr bool oi16(int64_t x) { return x << 48 >> 48 != x; }
inline constexpr bool oi32(int64_t x) { return x << 32 >> 32 != x; }


inline constexpr uint8_t  bu(uint64_t x) { return ou32(x) ? 3 : ou16(x) ? 2 : ou8(x) ? 1 : 0; }
inline constexpr uint8_t  bi(int64_t  x) { return oi32(x) ? 3 : oi16(x) ? 2 : oi8(x) ? 1 : 0; }

inline constexpr uint8_t  su(uint64_t x) { return 1 << bu(x); }
inline constexpr uint8_t  si(int64_t  x) { return 1 << bi(x); }


constexpr uint8_t ilog(uint64_t x)
{ uint8_t i = 0;
  for (uint8_t j = 32; j; j >>= 1) if (x >> j) i += j, x >>= j;
  return i; }


}


#include "../module/end.hh"

#endif
