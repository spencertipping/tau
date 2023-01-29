#include <memory>

#include "../dep/picosha3.h"

#include "arch.hh"
#if τhas_zstd
# include <zstd.h>
#endif


#include "eta0i.hh"
#include "begin.hh"

namespace τ
{


bool η0bc(u8c *a, uN s)
{
  if (!s)           return false;
  if (!(*a & 127))  return s >= 1u << (*a & 3);
  if (*a >> 6 == 2) return s >= 2u && s >= (a[1] & 127);
  return s >= 2u && s >= 2u + (*a & 7) && s >= η0i(a).osize();
}


Ar<u8, 32> η0i::sha3() const
{ auto sha3_256 = picosha3::get_sha3_generator<256>();
  Ar<u8, 32> hv{};
  let v = Bv{cdata(), csize()};
  sha3_256(v.begin(), v.end(), hv.begin(), hv.end());
  return hv; }


#if τhas_zstd
u8 *η0i::unzip(uN limit) const
{ let us = size();              A(us <= limit, "unzip() oversized data: " << us << " > " << limit);
  let r  = Sc<u8*>(malloc(us)); A(r,           "malloc() for decompression failed");
  let ds = ZSTD_decompress(r, us, cdata() + 8, csize() - 8);
  if (ZSTD_isError(ds)) { free(r); A(0, "η₀ corrupt compressed data: " << ZSTD_getErrorName(ds)); }
  if (ds != us)         { free(r); A(0, "η₀ decompressed size mismatch: " << ds << " ≠ " << us); }
  return r; }
#else
u8 *η0i::unzip(uN limit) const
{ A(0, "no zstd on this platform");
  return nullptr; }
#endif


O &operator<<(O &s, η0ft t)
{
  switch (t)
  {
  case η0ft::s: return s << "s";
  case η0ft::m: return s << "m";
  case η0ft::l: return s << "l";
  case η0ft::d: return s << "d";
  }
}

O &operator<<(O &s, η0i const &i)
{
  return s << "η₀[" << i.ftype()
           << (i.f() ? "F" : "f")
           << (i.c() ? "C" : "c")
           << (i.h() ? "H" : "h")
           << " t=" << i.type()
           << " S=" << i.osize() << "]";
}


}

#include "end.hh"
