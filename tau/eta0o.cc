#include <memory>
#include <string>

#include "../dep/picosha3.h"

#include "arch.hh"
#if τhas_zstd
# include <zstd.h>
#endif


#include "eta0o.hh"
#include "begin.hh"

namespace τ
{


void η0o::into(u8 *m) const
{
  let s = isize();
  let d = idata();
  uN  o = 0;

  switch (ft())
  {
  case η0ft::s:
    m[o++] = Sc<u8>(t_) << 2 | (s == 8 ? 3 : s == 4 ? 2 : s == 2 ? 1 : 0);
    break;
  case η0ft::m:
    m[o++] = 0x80 | Sc<u8>(t_);
    m[o++] = (f_ ? 0x80 : 0) | s;
    break;
  case η0ft::l:
  {
    let b = ubytes(std::max(Sc<uN>(1), s));
    m[o++] = 0xc0 | f_ << 4 | (c_ > 0) << 3 | b - 1;
    m[o++] = Sc<u8>(t_);
    for (u8 i = 0; i < b; ++i) m[o++] = s >> (b - i - 1) * 8 & 255;
    break;
  }
  case η0ft::d:
  {
    let b = ubytes(std::max(Sc<uN>(1), s));
    m[o++] = 0xe0 | h_ << 4 | (c_ > 0) << 3 | b - 1;
    m[o++] = Sc<u8>(t_);
    for (u8 i = 0; i < b; ++i) m[o++] = s >> (b - i - 1) * 8 & 255;
    break;
  }
  }

  if (h_)
  {
    auto sha3_256 = picosha3::get_sha3_generator<256>();
    Ar<u8, 32> hv{};
    sha3_256.process(d, d + s);
    sha3_256.finish();
    sha3_256.get_hash_bytes(hv.begin(), hv.end());
    memcpy(m + o, hv.data(), 32);
    o += 32;
  }

  memcpy(m + o, d, s);
  o += s;

#if τdebug
  let os = osize();
  A(o == os,
    "η₀o::into() size mismatch " << o << " ≠ " << os
    << "; c=" << c_ << " f=" << f_ << " h=" << h_
    << " ft=" << ft() << " is=" << isize());
#endif
}


B *η0o::compress() const
{
#if τhas_zstd
  if (cs_) return cs_;
  let r  = new B; r->resize(8 + ZSTD_compressBound(isize()));
  let cr = ZSTD_compress(r->data() + 8, r->capacity() - 8, idata(), isize(), c_);
  if (ZSTD_isError(cr))
  {
    delete r;
    A(0, "η₀ compression error: " << ZSTD_getErrorName(cr));
  }
  r->resize(cr + 8);
  W(r->data(), 0, Sc<u64>(isize()));
  return r;
#else
  A(0, "no zstd on this platform");
  return nullptr;
#endif
}


O &operator<<(O &s, η0o const &x)
{
  return s << "TODO: << η₀o";
}


}

#include "end.hh"
