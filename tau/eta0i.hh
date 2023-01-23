#ifndef τη0i_h
#define τη0i_h


#include <memory>

#include <zstd.h>

#include "../dep/picosha3.h"


#include "types.hh"
#include "begin.hh"

namespace τ
{


typedef u8 η0t;


// η₀ frame type (not exported, just for internal logic):
// short, medium, long, disk
enum class η0ft : u8 { s, m, l, d };


// Bounds-check for a η₀ structure at the given address with the specified
// size bound -- returns true if the structure fits within that bound
//
// WARNING: never decode a η₀ from untrusted memory without calling this
// function first
bool η0bc(u8c*, uN);


// η₀ structure input reader: decodes the frame and provides .data() and
// .size(), as well as metadata.
//
// NOTE: η₀ .data() and .size() will transparently decompress; if you want
// compressed data with the length prefix, use .cdata() and .csize().
struct η0i
{
  η0i(u8c *a_) : d(nullptr) { *this = a; }
  η0i(η0i &&x) : a(x.a), d(x.d), ft(x.ft), hs(x.hs), t(x.t) { x.d = nullptr; }

  ~η0i() { if (d) free(d); }


  η0t  type()  const { return t; }
  η0ft ftype() const { return ft; }
  uN   hsize() const { return hs; }
  uN   osize() const { return hs + csize(); }
  u8c *cdata() const { return a + hs; }
  uN   csize() const
    { switch (ft)
      {
      case η0ft::s: return 1 << (*a & 3);
      case η0ft::m: return a[1] & 127;
      case η0ft::l:
      case η0ft::d:
      { uN s = 0;
        for (uN i = 0; i < (*a & 7) + 1; ++i) s = s << 8 | a[2 + i];
        return s; }
      } }

  u8c *data(uN limit = -1) const
    { if (!is_c()) return cdata();
      if (!d) d = unzip(limit);
      return d; }

  uN size() const
    { if (!is_c()) return csize();
      let s = csize();
      let c = cdata();
      A(s >= 8, "η₀ compressed data too small to contain length prefix: " << s);
      return Sc<u64>(c[0]) << 56 | Sc<u64>(c[1]) << 48
           | Sc<u64>(c[2]) << 40 | Sc<u64>(c[3]) << 32
           | Sc<u64>(c[4]) << 24 | Sc<u64>(c[5]) << 16
           | Sc<u64>(c[6]) << 8  | Sc<u64>(c[7]); }


  // Flags
  bool is_f() const
    { switch (ft)
      {
      case η0ft::s: return !t;
      case η0ft::m: return a[1] & 128;
      case η0ft::l: return *a & 16;
      case η0ft::d: return false;
      } }

  bool is_c() const
    { switch (ft)
      {
      case η0ft::s: case η0ft::m: return false;
      case η0ft::l: case η0ft::d: return a[0] & 8;
      } }

  bool is_h() const
    { switch (ft)
      {
      case η0ft::s:
      case η0ft::m:
      case η0ft::l: return false;
      case η0ft::d: return *a & 16;
      } }

  bool is_v() const
    { if (!is_h()) return true;
      auto sha3_256 = picosha3::get_sha3_generator<256>();
      Ar<u8, 32> hv{};
      sha3_256(cdata(), cdata() + csize(), hv.begin(), hv.end());
      return !memcmp(cdata() - 32, hv.data(), 32); }


  // Byte-level iteration
  u8c *begin() const { return data(); }
  u8c *end()   const { return data() + size(); }

  η0i &operator=(η0i const &x) { return *this = x.a; }
  η0i &operator=(u8c *a_)      { decode(a_); return *this; }


protected:
  u8c        *a;   // pointer to control byte
  u8 mutable *d;   // pointer to decompressed data
  η0ft        ft;  // frame type
  u8          hs;  // header size -- *a + hs == data
  η0t         t;   // type ID


  // Decompression: null if the compressed data exceeds the size limit
  u8 *unzip(uN limit = -1) const
    { let us = size();              if (us >= limit) return nullptr;
      let r  = Sc<u8*>(malloc(us)); if (!r)          return nullptr;
      let ds = ZSTD_decompress(r, us, cdata() + 8, csize() - 8);
      if (ZSTD_isError(ds)) { free(r); A(0, "η₀ corrupt compressed data: " << ZSTD_getErrorName(ds)); }
      if (ds != us)         { free(r); A(0, "η₀ decompressed size mismatch: " << ds << " ≠ " << us); }
      return r; }

  void decode(u8c *a_)
    { a = a_;
      if (d) free(d);
      d  = nullptr;
      ft = decode_ft();
      hs = calculate_hs();
      t  = decode_type(); }

  η0ft decode_ft() const
    { if (!(*a & 128))  return η0ft::s;
      if (*a >> 6 == 2) return η0ft::m;
      if (*a >> 5 == 6) return η0ft::l;
      return η0ft::d; }

  u8 calculate_hs() const
    { switch (ft)
      {
      case η0ft::s: return 1;
      case η0ft::m: return 2;
      case η0ft::l: return 2 + (*a & 7);
      case η0ft::d: return 2 + (*a & 7) + (is_h() ? 32 : 0);
      } }

  η0t decode_type() const
    { switch (ft)
      {
      case η0ft::s: return *a >> 2;
      case η0ft::m: return *a & 63;
      case η0ft::l:
      case η0ft::d: return a[1];
      } }
};


static_assert(sizeof(η0i) <= 3 * sizeof(uN));


bool η0bc(u8c *a, uN s)
{
  if (!s)           return false;
  if (!(*a & 127))  return s >= 1 << (*a & 3);
  if (*a >> 6 == 2) return s >= 2 && s >= (a[1] & 127);
  return s >= 2 && s >= 2 + (*a & 7) && s >= η0i(a).osize();
}


#if τdebug_iostream
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
           << (i.is_f() ? "F" : "f")
           << (i.is_c() ? "C" : "c")
           << (i.is_h() ? "H" : "h")
           << " t=" << Sc<uN>(i.type())
           << " s=" << i.size() << "]";
}
#endif


}

#include "end.hh"

#endif
