#ifndef τnumerics_h
#define τnumerics_h


#include <bit>
#include <limits>


#include "types.hh"


#include "begin.hh"

namespace τ
{


let nτ = 6.28318530717959;
let nφ = 0.5 + sqrt(5) / 2;


// Bytes required to store an integer
ic u8 ubytes(u64 x)
{
  u8 r = 0;
  if (x >> 32) r += 4, x >>= 32;
  if (x >> 16) r += 2, x >>= 16;
  if (x >> 8)  r += 1, x >>= 8;
  if (x) ++r;
  return r;
}

// Bits required to store an integer (i.e. index of highest bit)
ic u8 ubits(u64 x)
{
  u8 i = 0;
  for (u8 j = 32; j; j >>= 1) if (x >> j) i += j, x >>= j;
  return i;
}

static_assert(ubytes(0) == 0);
static_assert(ubytes(1) == 1);
static_assert(ubytes(255) == 1);
static_assert(ubytes(256) == 2);

static_assert(ubits(1) == 0);
static_assert(ubits(2) == 1);
static_assert(ubits(4) == 2);
static_assert(ubits(7) == 2);
static_assert(ubits(8) == 3);
static_assert(ubits(65535) == 15);
static_assert(ubits(65536) == 16);
static_assert(ubits(1ull << 32) == 32);


}

#include "end.hh"


#endif
