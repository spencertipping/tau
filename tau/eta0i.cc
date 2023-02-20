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
  if (!s)                                           return false;
  if (!(*a & 127) && s < 1u << (*a & 3))            return false;
  if (*a >> 6 == 2 && (s < 2u || s < (a[1] & 127))) return false;
  if (s < 2u || s < 2u + (*a & 7))                  return false;
  return η0i(a).osize() <= s;
}


bool η1bc(u8c *a, uN s)
{
  if (!η0bc(a, s)) return false;

  let y = η0i(a);
  if (η1tc[y.type()])
  {
    let  x0 = y.data();
    let  s  = y.size();
    u8c *x  = x0;
    uN   n  = 0;  // number of children

    while (x < x0 + s)
    {
      if (!η0bc(x, s - (x - x0))) return false;
      x += η0i(x).osize();
      ++n;
    }

    // Maps must have an even number of children; otherwise k/v lookups will
    // overflow buffers
    if (y.type() == η0t::map && n & 1) return false;

    // Child size must exactly equal container size, otherwise tuple lookups
    // will overflow buffers
    if (x != x0 + s) return false;
  }

  return true;
}


Ar<u8, 32> η0i::sha3() const
{
  auto sha3_256 = picosha3::get_sha3_generator<256>();
  Ar<u8, 32> hv{};
  let v = Bv{cdata(), csize()};
  sha3_256(v.begin(), v.end(), hv.begin(), hv.end());
  return hv;
}


#if τhas_zstd
u8 *η0i::unzip(uN limit) const
{
  let us = size();              A(us <= limit, "unzip() oversized data: " << us << " > " << limit);
  let r  = Sc<u8*>(malloc(us)); A(r,           "malloc() for decompression failed");
  let ds = ZSTD_decompress(r, us, cdata() + 8, csize() - 8);
  if (ZSTD_isError(ds)) { free(r); A(0, "η₀ corrupt compressed data: " << ZSTD_getErrorName(ds)); }
  if (ds != us)         { free(r); A(0, "η₀ decompressed size mismatch: " << ds << " ≠ " << us); }
  return r;
}
#else
u8 *η0i::unzip(uN limit) const
{
  A(0, "no zstd on this platform");
  return nullptr;
}
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
