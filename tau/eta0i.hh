#ifndef τη0i_h
#define τη0i_h


#include <cstring>


#include "types.hh"
#include "numerics.hh"
#include "eta-types.hh"

#include "begin.hh"

namespace τ
{


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
  η0i(u8c *a_) : d(nullptr) { *this = a_; }
  η0i(η0i &&x) : a(x.a), d(x.d), ft(x.ft), hs(x.hs), t(x.t) { x.d = nullptr; }

  ~η0i() { if (d) free(d); }


  η0t  type()  const { return t; }
  η0ft ftype() const { return ft; }
  uN   hsize() const { return hs; }
  uN   osize() const { return hs + csize(); }
  u8c *odata() const { return a; }
  u8c *cdata() const { return a + hs; }
  uN   csize() const
    { switch (ft)
      {
      case η0ft::s: return 1 << (*a & 3);
      case η0ft::m: return a[1] & 127;
      case η0ft::l:
      case η0ft::d:
      { uN s = 0;
        for (int i = 0; i < (*a & 7) + 1; ++i) s = s << 8 | a[2 + i];
        return s; }
      } }


  u8c *data(uN limit = -1) const
    { if (!c()) return cdata();
      if (!d) d = unzip(limit);
      return d; }

  uN size() const
    { if (!c()) return csize();
      let s = csize();
      let c = cdata();
      A(s >= 8, "η₀ compressed data too small to contain length prefix: " << s);
      return R<u64>(c, 0); }

  Bv  const bv (uN limit = -1) const { return Bv {data(limit),           size()}; }
  Stv const stv(uN limit = -1) const { return Stv{Rc<chc*>(data(limit)), size()}; }


  // Flags
  bool f() const
    { switch (ft)
      {
      case η0ft::s: return !Sc<u8>(t);
      case η0ft::m: return a[1] & 128;
      case η0ft::l: return *a & 16;
      case η0ft::d: return false;
      } }

  bool c() const
    { switch (ft)
      {
      case η0ft::s: case η0ft::m: return false;
      case η0ft::l: case η0ft::d: return a[0] & 8;
      } }

  bool h() const
    { switch (ft)
      {
      case η0ft::s:
      case η0ft::m:
      case η0ft::l: return false;
      case η0ft::d: return *a & 16;
      } }

  bool v() const { return !h() || sha3() == stored_sha3(); }


  Ar<u8, 32> sha3() const;
  Ar<u8, 32> stored_sha3() const
    { A(h(), "stored_sha3 on non-hashed frame");
      Ar<u8, 32> r;
      memcpy(r.data(), cdata() - 32, 32);
      return r; }


  // Byte-level iteration (over decompressed data)
  u8c *begin() const { return data(); }
  u8c *end()   const { return data() + size(); }

  η0i &operator=(η0i const &x) { return *this = x.a; }
  η0i &operator=(u8c *a_)      { decode(a_); return *this; }


  // Copy verbatim into another buffer -- make sure the buffer is
  // at least osize() bytes long
  void into(u8 *m) const { memcpy(m, odata(), osize()); }


protected:
  u8c        *a;   // pointer to control byte
  u8 mutable *d;   // pointer to decompressed data
  η0ft        ft;  // frame type
  u8          hs;  // header size -- *a + hs == data
  η0t         t;   // type ID


  // Decompression: null if the compressed data exceeds the size limit
  u8 *unzip(uN limit = -1) const;

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
      case η0ft::l: return 2 + (*a & 7) + 1;
      case η0ft::d: return 2 + (*a & 7) + 1 + (h() ? 32 : 0);
      } }

  η0t decode_type() const
    { switch (ft)
      {
      case η0ft::s: return Sc<η0t>(*a >> 2);
      case η0ft::m: return Sc<η0t>(*a & 63);
      case η0ft::l:
      case η0ft::d: return Sc<η0t>(a[1]);
      } }
};


static_assert(sizeof(η0i) <= 3 * sizeof(uN));


O &operator<<(O &s, η0ft t);
O &operator<<(O &s, η0i const &i);


}

#include "end.hh"

#endif
