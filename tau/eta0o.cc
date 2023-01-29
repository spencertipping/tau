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
  let b = ubytes(std::max(Sc<uN>(1), s));
  uN  o = 0;

  switch (ft())
  {
  case η0ft::s:
    m[o++] = Sc<u8>(t_) << 2 | ubits(s);
    break;
  case η0ft::m:
    m[o++] = 0x80 | Sc<u8>(t_);
    m[o++] = (f_ ? 0x80 : 0) | s;
    break;
  case η0ft::l:
    m[o++] = 0xc0 | f_ << 4 | (c_ > 0) << 3 | b - 1;
    m[o++] = Sc<u8>(t_);
    for (u8 i = 0; i < b; ++i) m[o++] = s >> (b - i - 1) * 8 & 255;
    break;
  case η0ft::d:
    m[o++] = 0xe0 | h_ << 4 | (c_ > 0) << 3 | b - 1;
    m[o++] = Sc<u8>(t_);
    for (u8 i = 0; i < b; ++i) m[o++] = s >> (b - i - 1) * 8 & 255;
    break;
  }

  if (h_)
  {
    auto sha3_256 = picosha3::get_sha3_generator<256>();
    if (c_)  // hash the length prefix
    { u8 p[8]; W(p, 0, Sc<u64>(sd().size()));
      sha3_256.process(std::begin(p), std::end(p)); }
    Ar<u8, 32> hv{};
    sha3_256.process(data().begin(), data().end());
    sha3_256.finish();
    sha3_256.get_hash_bytes(hv.begin(), hv.end());
    memcpy(m + o, hv.data(), 32);
    o += 32;
  }

  if (c_)
  { W(m, o, Sc<u64>(sd().size()));
    o += 8; }

  switch (t_)
  {
  case η0t::η0:       *Rc<void**>(m + o) = d.p.p; o += sizeof(void*);                   break;
  case η0t::int_be:   for (u8 i = 0; i < s; ++i) m[o++] = d.p.i >> (s - i - 1)*8 & 255; break;
  case η0t::uint_be:  for (u8 i = 0; i < s; ++i) m[o++] = d.p.u >> (s - i - 1)*8 & 255; break;
  case η0t::float_be: W(m, o, d.p.f); o += sizeof(d.p.f);                               break;
  case η0t::boolean:  W(m, o++, Sc<u8>(d.p.b ? 1 : 0));                                 break;
  default:
  { let x = data();
    memcpy(m + o, x.data(), x.size());
    o += x.size();
    break; }
  }

  let os = osize();
  A(o == os,
    "η₀o::into() size mismatch " << o << " ≠ " << os
    << "; c=" << c_ << " f=" << f_ << " h=" << h_
    << " ft=" << ft() << " is=" << isize());
}


B *η0o::cdata() const
{
#if τhas_zstd
  A(!p(), "cdata() on primitive");
  let r  = new B; r->resize(ZSTD_compressBound(d.s_->size()));
  let cr = ZSTD_compress(r   ->data(), r   ->capacity(),
                         d.s_->data(), d.s_->size(), c_);
  if (ZSTD_isError(cr))
  { delete r;
    A(0, "η₀ compression error: " << ZSTD_getErrorName(cr)); }
  r->resize(cr);
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
