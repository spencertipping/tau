#ifndef tau_utf9_h
#define tau_utf9_h


#include <algorithm>
#include <bit>
#include <cstdint>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <byteswap.h>

#define XXH_INLINE_ALL 1
#include "xxhash.h"
#include "picosha2.h"


namespace tau::utf9
{

#define let auto const

#define LE (std::endian::native == std::endian::little)
#define BE (std::endian::native == std::endian::big)

static_assert(LE || BE, "unsupported endianness");

static_assert(sizeof(long)   == sizeof(uint64_t));
static_assert(sizeof(double) == sizeof(uint64_t));
static_assert(sizeof(float)  == sizeof(uint32_t));
static_assert(sizeof(void*)  == sizeof(uint64_t));
static_assert(sizeof(char)   == sizeof(uint8_t));

static_assert(sizeof(XXH64_hash_t) == sizeof(uint64_t));


struct ibuf;
struct obuf;
struct tval;

struct val;

#define U9 (tau::utf9::val)


struct utf9_error       : public std::exception { std::string const &m;    utf9_error(std::string const &m_) : m(m_) {} };
struct internal_error   : public utf9_error {                              internal_error(std::string const &m_) : utf9_error(m_) {} };
struct decoding_error   : public utf9_error { ibuf const &b; uint64_t i;   decoding_error(std::string const &m_, ibuf const &b_, uint64_t i_) : utf9_error(m_), b(b_), i(i_) {} };
struct toperation_error : public utf9_error { tval const &t;               toperation_error(std::string const &m_, tval const &t_) : utf9_error(m_), t(t_) {} };
struct voperation_error : public utf9_error { val  const &v;               voperation_error(std::string const &m_, val  const &v_) : utf9_error(m_), v(v_) {} };
struct binop_error      : public utf9_error { val  const &a; val const &b; binop_error(std::string const &m_, val const &a_, val const &b_) : utf9_error(m_), a(a_), b(b_) {} };
struct encoding_error   : public utf9_error { tval const &t; val const &v; encoding_error(std::string const &m_, tval const &t_, val const &v_) : utf9_error(m_), t(t_), v(v_) {} };


namespace  // error to ostream
{

std::ostream &operator<<(std::ostream&, tval const&);
std::ostream &operator<<(std::ostream&, val  const&);

std::ostream &operator<<(std::ostream &s, utf9_error       const &e) { return s << e.m; }
std::ostream &operator<<(std::ostream &s, decoding_error   const &e) { return s << e.m << " @ " << e.i; }
std::ostream &operator<<(std::ostream &s, toperation_error const &e) { return s << e.m << " @ " << e.t; }
std::ostream &operator<<(std::ostream &s, voperation_error const &e) { return s << e.m << " @ " << e.v; }
std::ostream &operator<<(std::ostream &s, binop_error      const &e) { return s << e.m << " @ " << e.a << ", " << e.b; }
std::ostream &operator<<(std::ostream &s, encoding_error   const &e) { return s << e.m << " @ " << e.t << " : " << e.v; }

}


namespace  // Consistent-endianness (i.e. big-endian in memory)
{

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

}


namespace  // overflow checks
{

inline bool ou8 (uint64_t x) { return !(x >> 8); }
inline bool ou16(uint64_t x) { return !(x >> 16); }
inline bool ou32(uint64_t x) { return !(x >> 32); }

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


inline uint64_t xxh(void const * x, size_t l, uint64_t s) { return XXH64(x, l, s); }
inline uint64_t xxc(uint64_t a, uint64_t b)               { a = ce(a); return XXH64(&a, sizeof a, b); }


namespace  // Dispatch table compression
{

typedef int32_t tpacked;

void pack_table(uint64_t const *       t,
                uint64_t const         base,
                tpacked        * const p,
                int              const n)
{
  for (int i = 0; i < n; ++i)
  { p[i] = static_cast<tpacked>(t[i] - base);
    if (p[i] + base != t[i]) throw std::length_error("pack_table overflow"); }
}

}


enum val_type : uint8_t
{
  UINT     = 0x00,
  INT      = 0x04,
  FLOAT32  = 0x08,
  FLOAT64  = 0x09,
  SYMBOL   = 0x0a,
  PIDFD    = 0x0b,
  BOOL     = 0x0c,
  NULLTYPE = 0x0e,

  Α        = 0x10,
  Ω        = 0x11,
  Ι        = 0x12,
  Κ        = 0x13,
  Τ        = 0x14,
  Ρ        = 0x15,
  Θ        = 0x16,

  UTF8     = 0x18,
  BYTES    = 0x1c,

  TUPLE    = 0x20,
  ARRAY    = 0x24,
  INDEX    = 0x30,

  NONE     = 0x3e,  // fictitious: not a real value
  BOGUS    = 0x3f,
};

typedef uint64_t val_type_mask;


// A bytecode decoder with fully-buffered and bounded source data.
struct ibuf
{
  uint8_t const * const xs;
  uint64_t        const l;
  bool            const owned;

  ibuf(uint8_t const *xs_, uint64_t l_, bool owned_ = false)
    : xs(xs_), l(l_), owned(owned_) {}

  ~ibuf() { if (owned) delete[] xs; }


  bool has  (uint64_t i) const { return i < l; }
  void check(uint64_t i) const { if (!has(i)) throw decoding_error("ibuf bounds", *this, i); }

  uint64_t len  (uint64_t i) const;
  uint64_t tlen (uint64_t i) const;
  uint64_t tvlen(uint64_t i) const;


  uint8_t const *operator+(uint64_t i) const { return xs + i; }


  uint8_t  u8 (uint64_t i) const { return xs[i]; }
  uint16_t u16(uint64_t i) const { return ce(*reinterpret_cast<uint16_t const*>(xs + i)); }
  uint32_t u32(uint64_t i) const { return ce(*reinterpret_cast<uint32_t const*>(xs + i)); }
  uint64_t u64(uint64_t i) const { return ce(*reinterpret_cast<uint64_t const*>(xs + i)); }

  int8_t   i8 (uint64_t i) const { return reinterpret_cast<int8_t const*>(xs)[i]; }
  int16_t  i16(uint64_t i) const { return ce(*reinterpret_cast<int16_t const*>(xs + i)); }
  int32_t  i32(uint64_t i) const { return ce(*reinterpret_cast<int32_t const*>(xs + i)); }
  int64_t  i64(uint64_t i) const { return ce(*reinterpret_cast<int64_t const*>(xs + i)); }

  uint8_t  ci8 (uint64_t i) const { check(i);     return i8(i);  }
  uint16_t ci16(uint64_t i) const { check(i + 1); return i16(i); }
  uint32_t ci32(uint64_t i) const { check(i + 3); return i32(i); }
  uint64_t ci64(uint64_t i) const { check(i + 7); return i64(i); }

  uint8_t  cu8 (uint64_t i) const { check(i);     return u8(i);  }
  uint16_t cu16(uint64_t i) const { check(i + 1); return u16(i); }
  uint32_t cu32(uint64_t i) const { check(i + 3); return u32(i); }
  uint64_t cu64(uint64_t i) const { check(i + 7); return u64(i); }

  float  f32(uint64_t i) const { return ce(*reinterpret_cast<float const*>(xs + i)); }
  double f64(uint64_t i) const { return ce(*reinterpret_cast<double const*>(xs + i)); }
};


// A bytecode encoder with no specified storage backend
struct oenc
{
  virtual ~oenc() {};

  virtual void     ensure_capacity(uint64_t l_) = 0;
  virtual void     push(uint8_t x) = 0;
  virtual void     write(uint8_t const *xs, uint64_t n) = 0;
  virtual void     move(uint64_t from, uint64_t to, uint64_t n) = 0;
  virtual void     seek(uint64_t to) = 0;
  virtual uint64_t size() const = 0;
  virtual void     fill(uint8_t c, uint64_t n) = 0;

  oenc &u8 (uint8_t  x) { push(x); return *this; }
  oenc &u16(uint16_t x) { u8 (x >> 8);  return u8 (x & 0xff); }
  oenc &u32(uint32_t x) { u16(x >> 16); return u16(x & 0xffff); }
  oenc &u64(uint64_t x) { u32(x >> 32); return u32(x & 0xffffffff); }
  oenc &f32(float    x) { union { float xf;  uint32_t xi; }; xf = x; return u32(xi); }
  oenc &f64(double   x) { union { double xd; uint64_t xi; }; xd = x; return u64(xi); }

  oenc &xs(uint8_t const *xs, uint64_t n) { write(xs, n); return *this; }

  oenc &operator<<(uint8_t  x)    { return u8(x);  }
  oenc &operator<<(uint16_t x)    { return u16(x); }
  oenc &operator<<(uint32_t x)    { return u32(x); }
  oenc &operator<<(uint64_t x)    { return u64(x); }
  oenc &operator<<(float    x)    { return f32(x); }
  oenc &operator<<(double   x)    { return f64(x); }
  oenc &operator<<(ibuf const &b) { return xs(b.xs, b.l); }

  template <class T>
  oenc &operator<<(std::basic_string<T> x)
    { static_assert(sizeof x.data()[0] == 1);
      write(reinterpret_cast<uint8_t*>(x.data()), x.size());
      return *this; }

  template <class T>
  oenc &operator<<(std::basic_string_view<T> x)
    { static_assert(sizeof x.data()[0] == 1);
      write(reinterpret_cast<uint8_t*>(x.data()), x.size());
      return *this; }
};


// A fully buffered bytecode encoder
struct obuf : public oenc
{
  uint8_t * b = nullptr;  // buffer data (will be moved)
  uint64_t  l = 0;        // allocated size
  uint64_t  i = 0;        // position of next byte to be written

  virtual ~obuf() { if (b) delete[] b;}


  ibuf convert_to_ibuf()
    { let r = ibuf(b, i, true);
      b = nullptr;
      l = i = 0;
      return r; }


  uint8_t *data()     const { return b; }
  uint64_t size()     const { return i; }
  uint64_t capacity() const { return l; }

  void push(uint8_t x)                      { ensure_capacity(i + 1); b[i++] = x; }
  void write(uint8_t const *xs, uint64_t n) { ensure_capacity(i + n); memcpy(b + i, xs, n); i += n; }
  void fill(uint8_t c, uint64_t n)          { ensure_capacity(i + n); memset(b + i,  c, n); i += n; }

  void seek(uint64_t to) { i = to; }
  void move(uint64_t from, uint64_t to, uint64_t n)
    { if (to > from) ensure_capacity(l + (to - from));
      std::memmove(b + to, b + from, n); }

  void ensure_capacity(uint64_t l_)
    { if (b == nullptr) { b = new uint8_t[1024]; l = 1024; }
      else if (l_ > l)
      { uint64_t nl = l; while (nl < l_) nl <<= 1;
        uint8_t *b_ = new uint8_t[nl];
        memcpy(b_, b, i);
        delete[] b;
        b = b_;
        l = nl; } }
};


namespace  // Length traversal dispatch
{

typedef uint64_t(*lfn)(ibuf const &, uint64_t);
#define lf(body) [](ibuf const &b, uint64_t i) -> uint64_t { return static_cast<uint64_t>(body); }


let l1  = lf(1);
let l2  = lf(2);
let l3  = lf(3);
let l4  = lf(4);
let l5  = lf(5);
let l6  = lf(6);
let l7  = lf(7);
let l8  = lf(8);
let l9  = lf(9);
let l17 = lf(17);


// Value-length functions
let str8_lf   = lf(2 + b.u8 (i + 1));
let str16_lf  = lf(3 + b.u16(i + 1));
let str32_lf  = lf(5 + b.u32(i + 1));
let str64_lf  = lf(9 + b.u64(i + 1));

let list8_lf  = lf(3  + b.u8 (i + 1));
let list16_lf = lf(5  + b.u16(i + 1));
let list32_lf = lf(9  + b.u32(i + 1));
let list64_lf = lf(17 + b.u64(i + 1));

let u8_tvlf  = lf(b.u8 (i + 1));
let u16_tvlf = lf(b.u16(i + 1));
let u32_tvlf = lf(b.u32(i + 1));
let u64_tvlf = lf(b.u64(i + 1));

let fixutf8_lf   = lf(1 + (b.u8(i) - 0x20));
let fixbytes_lf  = lf(1 + (b.u8(i) - 0x30));
let fixtuple8_lf = lf(2 + b.u8(i + 1));
let fixint_lf    = l1;

let idx16_lf = lf(1 + 2 + b.u16(i + 1));
let idx32_lf = lf(1 + 4 + b.u32(i + 1));
let idx64_lf = lf(1 + 8 + b.u64(i + 1));

let bogus_lf = [](ibuf const &b, uint64_t i) -> uint64_t { throw decoding_error("bogus lf", b, i); };


// Typecode length functions
inline uint64_t tuple_tl(ibuf const &b, uint64_t i, uint64_t n)
{
  uint64_t l = 0;
  while (n--) l += b.tlen(i + l);
  return l;
}

let fixtuple_tlf = lf(2 + tuple_tl(b, i + 2, b.u8(i) - 0x48));


// Typecode value length functions
let fixutf8_tvlf   = lf(b.u8(i) - 0x20);
let fixbytes_tvlf  = lf(b.u8(i) - 0x30);
let fixtuple8_tvlf = u8_tvlf;


// Bytecode decode tables
lfn const lfns[256] =
{
  // 0x00 - 0x0f
  l2, l3, l5, l9,
  l2, l3, l5, l9,
  l5, l9, l9, l9,
  l1, l1, l1, bogus_lf,

  // 0x10 - 0x1f
  l1, l1, l1, l1,
  l1,  // tau
  l9,  // rho
  l9,  // theta
  bogus_lf,

  str8_lf, str16_lf, str32_lf, str64_lf,
  str8_lf, str16_lf, str32_lf, str64_lf,

  // 0x2N
  fixutf8_lf, fixutf8_lf, fixutf8_lf, fixutf8_lf,
  fixutf8_lf, fixutf8_lf, fixutf8_lf, fixutf8_lf,
  fixutf8_lf, fixutf8_lf, fixutf8_lf, fixutf8_lf,
  fixutf8_lf, fixutf8_lf, fixutf8_lf, fixutf8_lf,

  // 0x3N
  fixbytes_lf, fixbytes_lf, fixbytes_lf, fixbytes_lf,
  fixbytes_lf, fixbytes_lf, fixbytes_lf, fixbytes_lf,
  fixbytes_lf, fixbytes_lf, fixbytes_lf, fixbytes_lf,
  fixbytes_lf, fixbytes_lf, fixbytes_lf, fixbytes_lf,

  // 0x40-0x4f
  list8_lf, list16_lf, list32_lf, list64_lf,
  list8_lf, list16_lf, list32_lf, list64_lf,
  fixtuple8_lf, fixtuple8_lf, fixtuple8_lf, fixtuple8_lf,
  fixtuple8_lf, fixtuple8_lf, fixtuple8_lf, fixtuple8_lf,

  // 0x50-0x5f
  lf(1 + 2 + 1 + b.u16(i + 1)),
  lf(1 + 4 + 1 + b.u32(i + 1)),
  lf(1 + 8 + 1 + b.u64(i + 1)),
  l1,

  idx16_lf, idx16_lf, idx16_lf, idx16_lf,
  idx16_lf, idx16_lf, idx16_lf, idx16_lf,
  idx16_lf, idx16_lf, idx16_lf, idx16_lf,

  // 0x60-0x6f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  idx32_lf, idx32_lf, idx32_lf, idx32_lf,
  idx32_lf, idx32_lf, idx32_lf, idx32_lf,
  idx32_lf, idx32_lf, idx32_lf, idx32_lf,

  // 0x70-0x7f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  idx64_lf, idx64_lf, idx64_lf, idx64_lf,
  idx64_lf, idx64_lf, idx64_lf, idx64_lf,
  idx64_lf, idx64_lf, idx64_lf, idx64_lf,

  // 0x80-0xbf
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  // 0xc0-0xff
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
};

lfn const tlfns[256] =
{
  // 0x00-0x0f
  l1, l1, l1, l1,
  l1, l1, l1, l1,
  l1, l1, l1, l1,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x10-0x1f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  l2, l3, l5, l9,
  l2, l3, l5, l9,

  // 0x20-0x2f
  l1, l1, l1, l1,
  l1, l1, l1, l1,
  l1, l1, l1, l1,
  l1, l1, l1, l1,

  // 0x30-0x3f
  l1, l1, l1, l1,
  l1, l1, l1, l1,
  l1, l1, l1, l1,
  l1, l1, l1, l1,

  // 0x40-0x4f
  lf(3  + tuple_tl(b, i + 3,  b.u8 (i + 2))),
  lf(5  + tuple_tl(b, i + 5,  b.u16(i + 3))),
  lf(9  + tuple_tl(b, i + 9,  b.u32(i + 5))),
  lf(17 + tuple_tl(b, i + 17, b.u64(i + 9))),

  lf(3  + b.tlen(i + 3)),
  lf(5  + b.tlen(i + 5)),
  lf(9  + b.tlen(i + 9)),
  lf(17 + b.tlen(i + 17)),

  fixtuple_tlf, fixtuple_tlf, fixtuple_tlf, fixtuple_tlf,
  fixtuple_tlf, fixtuple_tlf, fixtuple_tlf, fixtuple_tlf,

  // 0x50-0x5f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x60-0x6f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x70-0x7f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x80-0xbf
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0xc0-0xff
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
};

lfn const tvlfns[256] =
{
  // 0x00-0x0f
  l1, l2, l4, l8,
  l1, l2, l4, l8,
  l4, l8, l8, l8,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x10-0x1f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  u8_tvlf, u16_tvlf, u32_tvlf, u64_tvlf,
  u8_tvlf, u16_tvlf, u32_tvlf, u64_tvlf,

  // 0x20-0x2f
  fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf,
  fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf,
  fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf,
  fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf,

  // 0x30-0x3f
  fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf,
  fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf,
  fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf,
  fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf,

  // 0x40-0x4f
  u8_tvlf, u16_tvlf, u32_tvlf, u64_tvlf,
  u8_tvlf, u16_tvlf, u32_tvlf, u64_tvlf,
  fixtuple8_tvlf, fixtuple8_tvlf, fixtuple8_tvlf, fixtuple8_tvlf,
  fixtuple8_tvlf, fixtuple8_tvlf, fixtuple8_tvlf, fixtuple8_tvlf,

  // 0x50-0x5f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x60-0x6f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x70-0x7f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x80-0xbf
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0xc0-0xff
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
};


tpacked lfnos[256];
tpacked tlfnos[256];
tpacked tvlfnos[256];


uint64_t const lfn_base = reinterpret_cast<uint64_t>(&l1);


void init_lfn_tables()
{
  pack_table(reinterpret_cast<uint64_t const*>(lfns),   lfn_base, lfnos,   256);
  pack_table(reinterpret_cast<uint64_t const*>(tlfns),  lfn_base, tlfnos,  256);
  pack_table(reinterpret_cast<uint64_t const*>(tvlfns), lfn_base, tvlfnos, 256);
}


#undef lf

}

inline uint64_t ibuf::len  (uint64_t i) const { return reinterpret_cast<lfn>(lfn_base + lfnos  [xs[i]])(*this, i); }
inline uint64_t ibuf::tlen (uint64_t i) const { return reinterpret_cast<lfn>(lfn_base + tlfnos [xs[i]])(*this, i); }
inline uint64_t ibuf::tvlen(uint64_t i) const { return reinterpret_cast<lfn>(lfn_base + tvlfnos[xs[i]])(*this, i); }


namespace  // Type dispatch tables
{

val_type const bts[256] =
{
  // 0x00-0x0f
  UINT, UINT, UINT, UINT,
  INT,  INT,  INT,  INT,
  FLOAT32, FLOAT64, SYMBOL, PIDFD,
  BOOL, BOOL, NULLTYPE, BOGUS,

  // 0x10-0x1f
  Α, Ω, Ι, Κ,
  Τ, Ρ, Θ, BOGUS,
  UTF8, UTF8, UTF8, UTF8,
  BYTES, BYTES, BYTES, BYTES,

  // 0x20-0x2f
  UTF8, UTF8, UTF8, UTF8,
  UTF8, UTF8, UTF8, UTF8,
  UTF8, UTF8, UTF8, UTF8,
  UTF8, UTF8, UTF8, UTF8,

  // 0x30-0x3f
  BYTES, BYTES, BYTES, BYTES,
  BYTES, BYTES, BYTES, BYTES,
  BYTES, BYTES, BYTES, BYTES,
  BYTES, BYTES, BYTES, BYTES,

  // 0x40-0x4f
  TUPLE, TUPLE, TUPLE, TUPLE,
  ARRAY, ARRAY, ARRAY, ARRAY,
  TUPLE, TUPLE, TUPLE, TUPLE,
  TUPLE, TUPLE, TUPLE, TUPLE,

  // 0x50-0x5f
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,

  // 0x60-0x6f
  BOGUS, BOGUS, BOGUS, BOGUS,
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,

  // 0x70-0x7f
  BOGUS, BOGUS, BOGUS, BOGUS,
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,

  // 0x80-0xbf
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  // 0xc0-0xff
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
};


#define pf(body) [](ibuf const &b, uint64_t i) -> uint8_t const* { return (body); }

typedef uint8_t const*(*pfn)(ibuf const &, uint64_t);


let bogus_pf = [](ibuf const &b, uint64_t i) -> uint8_t const* { throw decoding_error("bogus pf", b, i); };

let p1  = pf(b + (i + 1));
let p2  = pf(b + (i + 2));
let p3  = pf(b + (i + 3));
let p4  = pf(b + (i + 4));
let p5  = pf(b + (i + 5));
let p6  = pf(b + (i + 6));
let p9  = pf(b + (i + 9));
let p10 = pf(b + (i + 10));
let p17 = pf(b + (i + 17));

let i16_pf = pf(b + (i + 5  + b.tlen(i + 5)));
let i32_pf = pf(b + (i + 9  + b.tlen(i + 9)));
let i64_pf = pf(b + (i + 17 + b.tlen(i + 17)));


pfn const sfns[256] =
{
  // 0x00-0x0f
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  // 0x10-0x1f
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  p2, p3, p5, p9,
  p2, p3, p5, p9,

  // 0x20-0x2f
  p1, p1, p1, p1,
  p1, p1, p1, p1,
  p1, p1, p1, p1,
  p1, p1, p1, p1,

  // 0x30-0x3f
  p1, p1, p1, p1,
  p1, p1, p1, p1,
  p1, p1, p1, p1,
  p1, p1, p1, p1,

  // 0x40-0x4f
  p3, p5, p9, p17,
  pf(b + (i + 3  + b.tlen(i + 3))),
  pf(b + (i + 5  + b.tlen(i + 5))),
  pf(b + (i + 9  + b.tlen(i + 9))),
  pf(b + (i + 17 + b.tlen(i + 17))),
  p2, p2, p2, p2,
  p2, p2, p2, p2,

  // 0x50-0x5f
  p4, p6, p10, bogus_pf,
  i16_pf, i16_pf, i16_pf, i16_pf,
  i16_pf, i16_pf, i16_pf, i16_pf,
  i16_pf, i16_pf, i16_pf, i16_pf,

  // 0x60-0x6f
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  i32_pf, i32_pf, i32_pf, i32_pf,
  i32_pf, i32_pf, i32_pf, i32_pf,
  i32_pf, i32_pf, i32_pf, i32_pf,

  // 0x70-0x7f
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  i64_pf, i64_pf, i64_pf, i64_pf,
  i64_pf, i64_pf, i64_pf, i64_pf,
  i64_pf, i64_pf, i64_pf, i64_pf,

  // 0x80-0xbf
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  // 0xc0-0xff
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
};

tpacked sfnos[256];

uint64_t const sfn_base = reinterpret_cast<uint64_t>(&p1);


void init_sfn_tables()
{
  pack_table(reinterpret_cast<uint64_t const *>(sfns), sfn_base, sfnos, 256);
}


#undef pf

}


struct sym
{
  uint64_t h;
  sym(std::string const &s) : h(std::hash<std::string>{}(s)) {}
  sym(uint64_t h_)          : h(h_)                          {}
  operator uint64_t() const { return h; }
};

struct hash
{
  uint64_t h;
  hash(uint64_t h_) : h(h_) {}
  operator uint64_t() const { return h; }
  int compare(hash const x) const { return h > x.h ? 1 : h < x.h ? -1 : 0; }
};

struct pidfd
{
  uint32_t pid;
  uint32_t fd;

  pidfd(uint32_t pid_, uint32_t fd_) : pid(pid_), fd(fd_) {}

  operator uint64_t() const { return (uint64_t) pid << 32 | fd; }

  bool operator<(pidfd const &v) const { return pid < v.pid || pid == v.pid && fd < v.fd; }
  bool operator>(pidfd const &v) const { return pid > v.pid || pid == v.pid && fd > v.fd; }
};


// A typecode beginning at a specific bytecode location
struct tval
{
  ibuf const * const b;
  uint64_t     const i;
  uint64_t     const e;

  tval(ibuf const &b_, uint64_t i_) : b(&b_), i(i_), e(i + b->tlen(i)) {}


  struct it
  {
    ibuf const * const b;
    uint64_t           i;

    tval operator* ()            const { return tval(*b, i); }
    it  &operator++()                  { i += b->tlen(i); return *this; }
    bool operator==(it const &x) const { return b == x.b && i == x.i; }
  };


  it begin() const
    { if (type() != TUPLE) throw toperation_error("begin()", *this);
      if (typecode() >= 0x48 && typecode() <= 0x4f) return it{b, i + 2};
      switch (typecode())
      {
      case 0x40: return it{b, i + 3};
      case 0x41: return it{b, i + 5};
      case 0x42: return it{b, i + 9};
      case 0x43: return it{b, i + 17};
      default: throw internal_error("tval begin()");
      } }

  it end() const { return it{b, e}; }

  tval operator[](uint64_t i) const { it i_ = begin(); while (i--) ++i_; return *i_; }
  uint64_t offset_of(uint64_t i) const
    { if (type() != TUPLE) throw toperation_error("offset_of()", *this);
      if (!i) return 0;
      --i;
      uint64_t s = 0;
      it i_ = begin();
      while (i--) s += (*i_).vsize(), ++i_;
      return s; }


  uint8_t  typecode() const { return b->u8(i); }
  val_type type()     const { return bts[typecode()]; }
  uint64_t vsize()    const { return b->tvlen(i); }
  uint64_t tsize()    const { return b->tlen(i); }


  uint64_t len() const
    { if (type() != ARRAY && type() != TUPLE) throw toperation_error("len()", *this);
      if (typecode() >= 0x48 && typecode() <= 0x4f) return typecode() - 0x48;
      switch (typecode())
      {
      case 0x40: case 0x44: return b->u8(i + 2);
      case 0x41: case 0x45: return b->u16(i + 3);
      case 0x42: case 0x46: return b->u32(i + 5);
      case 0x43: case 0x47: return b->u64(i + 9);
      default: throw internal_error("tval len()");
      } }

  tval atype() const
    { if (type() != ARRAY) throw toperation_error("atype()", *this);
      switch (typecode())
      {
      case 0x44: return tval(*b, i + 3);
      case 0x45: return tval(*b, i + 5);
      case 0x46: return tval(*b, i + 9);
      case 0x47: return tval(*b, i + 17);
      default: throw internal_error("tval atype()");
      } }


  // NOTE: this implementation requires that all typecodes are canonicalized
  // before being emitted to bytecode (otherwise comparisons will be unstable)
  int compare(tval const &t) const
    { let n1 =   tsize();
      let n2 = t.tsize();
      return n1 > n2 ? 1 : n1 < n2 ? -1 : std::__memcmp(*b + i, *t.b + t.i, n1); }


  uint64_t h() const { return xxh(b + i, e - i, 1); }

  // Encode a value without any type prefixes
  oenc &pack(oenc&, val const&) const;
};


// TODO: typecode generator functions


// utf9 value, either bytecode-bound or immediate
// Instances of this fit into two machine words on 64-bit machines. As a result,
// we can pass them around by-value with no appreciable overhead (although we
// unfortunately have a destructor to consider).
struct val
{
  // ibuf const * will be 8-byte aligned, so tags with the low bit set are
  // type designators; see tagify() and tag_type() above.
  union
  { ibuf const * b;
    uint64_t     tag; } const;

  union
  { uint64_t                               i;
    uint64_t                               vu;
    int64_t                                vi;
    double                                 vd;
    float                                  vf;
    pidfd                                  vp;
    sym                                    vy;
    std::basic_string_view<uint8_t> const *vb;
    std::vector<val>                      *vt; } const;


  // Tagging bit schema:
  // type:8 | owned:1 | immediate:1
  static constexpr inline uint64_t tagify  (val_type t, bool own = false) { return static_cast<uint64_t>(t) << 2 | (own ? 2 : 0) | 1; }
  static constexpr inline val_type tag_type(uint64_t tag)                 { return static_cast<val_type>(tag >> 2); }


  // Immediate data structure ownership follows const-ness
  constexpr val(val const &v) : tag(v.tag & ~2), i(v.i) {}
            val(val &&v)      : tag(v.tag),      i(v.i) { v.tag &= ~2ul; }


  val(uint64_t tag_, uint64_t v_) : tag(tag_), vu(v_) {}

  val(ibuf const &b_, uint64_t i_) : b(&b_), i(i_)
    { if (reinterpret_cast<uint64_t>(b) & 7) throw internal_error("unaligned");
      b->check(i);
      b->check(i + b->len(i) - 1); }

  val(uint64_t vu_) : tag(tagify(UINT)),    vu(vu_) {}
  val(int64_t vi_)  : tag(tagify(INT)),     vi(vi_) {}
  val(double vd_)   : tag(tagify(FLOAT64)), vd(vd_) {}
  val(float vf_)    : tag(tagify(FLOAT32)), vf(vf_) {}
  val(sym vy_)      : tag(tagify(SYMBOL)),  vy(vy_) {}
  val(pidfd vp_)    : tag(tagify(PIDFD)),   vp(vp_) {}
  val(bool b_)      : tag(tagify(BOOL)),    vu(b_)  {}

  val(std::vector<val> *vt_) : tag(tagify(TUPLE, true)), vt(vt_) {}

  val(std::initializer_list<val> vs)
    : tag(tagify(TUPLE, true)),
      vt(new std::vector<val>(vs.begin(), vs.end())) {}

  val(std::string const &s, val_type t_ = UTF8)
    : tag(tagify(t_, true)),
      vb(reinterpret_cast<std::basic_string_view<uint8_t>*>(
           new std::string_view(s.begin(), s.end())))
    {}


  val(tval const &t_, ibuf const &b, uint64_t i)
    { switch (t_.typecode())
      {
      case 0x00: tag = tagify(UINT); vu = b.u8 (i); break;
      case 0x01: tag = tagify(UINT); vu = b.u16(i); break;
      case 0x02: tag = tagify(UINT); vu = b.u32(i); break;
      case 0x03: tag = tagify(UINT); vu = b.u64(i); break;

      case 0x04: tag = tagify(INT); vi = b.i8 (i); break;
      case 0x05: tag = tagify(INT); vi = b.i16(i); break;
      case 0x06: tag = tagify(INT); vi = b.i32(i); break;
      case 0x07: tag = tagify(INT); vi = b.i64(i); break;

      case 0x08: tag = tagify(FLOAT32); vf = b.f32(i); break;
      case 0x09: tag = tagify(FLOAT64); vd = b.f64(i); break;

      case 0x0a: tag = tagify(SYMBOL); vy = sym{b.u64(i)}; break;
      case 0x0b: tag = tagify(PIDFD);  vp = pidfd{b.u32(i), b.u32(i + 4)}; break;

      case 0x20: case 0x21: case 0x22: case 0x23:
      case 0x24: case 0x25: case 0x26: case 0x27:
      case 0x28: case 0x29: case 0x2a: case 0x2b:
      case 0x2c: case 0x2d: case 0x2e: case 0x2f:
      case 0x18:
      case 0x19:
      case 0x1a:
      case 0x1b: tag = tagify(UTF8, true); vb = new std::basic_string_view<uint8_t>(b + i, t_.vsize()); break;

      case 0x30: case 0x31: case 0x32: case 0x33:
      case 0x34: case 0x35: case 0x36: case 0x37:
      case 0x38: case 0x39: case 0x3a: case 0x3b:
      case 0x3c: case 0x3d: case 0x3e: case 0x3f:
      case 0x1c:
      case 0x1d:
      case 0x1e:
      case 0x1f: tag = tagify(BYTES, true); vb = new std::basic_string_view<uint8_t>(b + i, t_.vsize()); break;

      case 0x48: case 0x49: case 0x4a: case 0x4b:
      case 0x4c: case 0x4d: case 0x4e: case 0x4f:
      case 0x40:
      case 0x41:
      case 0x42:
      case 0x43:
        tag = tagify(TUPLE, true);
        vt  = new std::vector<val>;
        vt->reserve(t_.len());
        for (let &t : t_)
        { vt->push_back(val(t, b, i));
          i += t.vsize(); }
        break;

      case 0x44:
      case 0x45:
      case 0x46:
      case 0x47:
        // NOTE: this isn't an array because keeping it that way would require
        // extra type information that we don't have space to store. Instead,
        // arrays degrade to tuples when copied out.
        tag = tagify(TUPLE, true);
        vt  = new std::vector<val>;
        vt->reserve(t_.len());
        for (uint64_t j = 0; j < t_.len(); j++)
        { vt->push_back(val(t_.atype(), b, i));
          i += t_.atype().vsize(); }
        break;

      default: throw internal_error("vtypecode");
      } }


  ~val()
    { if (tag & 2)  // value is owned, so we need to free it
        // NOTE: if() above for performance; the switch below is slower and is
        // the uncommon case
        switch (tag)
        {
        case tagify(UTF8,  true): case tagify(BYTES, true): delete vb; break;
        case tagify(TUPLE, true): case tagify(ARRAY, true): delete vt; break;
        } }


  val &operator=(val &&x)      { tag = x.tag;      i = x.i; x.tag &= ~2; return *this; }
  val &operator=(val const &x) { tag = x.tag & ~2; i = x.i;              return *this; }


  bool     exists()                      const { return type() != NONE; }
  bool     has_ibuf()                    const { return !(tag & 1); }
  bool     is_immediate()                const { return !has_ibuf(); }
  val_type type()                        const { return has_ibuf() ? bts[b->u8(i)] : tag_type(tag); }
  void     require_ibuf()                const { if (!has_ibuf()) throw voperation_error("ibuf required", *this); }
  void     require_type(val_type_mask m) const { if (!(1ul << type() & m)) throw voperation_error("invalid type", *this); }
  uint64_t bsize()                       const { require_ibuf(); return b->len(i); }


  // Skip past all prefix indexes to get to the structure being indexed
  val list() const
    { require_ibuf();
      uint64_t j = i;
      while (bts[b->cu8(j)] == INDEX) j += b->len(j);
      return j; }


  uint64_t mlen() const { return mend() - mbegin(); }

  uint8_t const *mbegin() const
    { if (has_ibuf()) return reinterpret_cast<pfn>(sfn_base + sfnos[b->u8(i)])(*b, i);
      require_type(1ul << UTF8 | 1ul << BYTES);
      return vb->data(); }

  uint8_t const *mend() const
    { if (has_ibuf()) return *b + i + b->len(i);
      require_type(1ul << UTF8 | 1ul << BYTES);
      return vb->data() + vb->size(); }

  uint64_t ibegin() const { require_ibuf(); return mbegin() - b->xs; }
  uint64_t iend()   const { require_ibuf(); return mend()   - b->xs; }


  struct it
  {
    union
    { ibuf               const * const b;
      std::vector<val>::const_iterator vi; };

    uint64_t i;

    it(ibuf const *b_, uint64_t i_)          : b(b_),   i(i_)   {}
    it(std::vector<val>::const_iterator vi_) : vi(vi_), i(-1ul) {}

    bool is_v() const { return i == -1ul; }

    val  operator*()               const { return is_v() ? *vi : val(*b, i); }
    it  &operator++()                    { if (is_v()) ++vi; else i += b->len(i); return *this; }
    bool operator==(it const &rhs) const { return b == rhs.b && i == rhs.i; }
  };

  it begin() const {
    require_type(1ul << TUPLE | 1ul << ARRAY);
    return has_ibuf() ? it(b, ibegin()) : it(vt->begin()); }

  it end() const {
    require_type(1ul << TUPLE | 1ul << ARRAY);
    return has_ibuf() ? it(b, iend()) : it(vt->end()); }


  uint64_t len() const
    { switch (tag)
      {
      case tagify(UTF8,  true):
      case tagify(BYTES, true): return vb->size();

      case tagify(ARRAY, true):
      case tagify(TUPLE, true): return vt->size();

      default:
      {
        let x = b->u8(i);
        if (x >= 0x48 && x <= 0x4f) return x - 0x48;
        if (x >= 0x54 && x <= 0x5f) return b->u16(i + 1) >> 1;
        if (x >= 0x64 && x <= 0x6f) return b->u32(i + 1) >> 2;
        if (x >= 0x74 && x <= 0x7f) return b->u64(i + 1) >> 3;
        switch (x)
        {
        case 0x18: case 0x1c: return b->u8 (i + 1);
        case 0x19: case 0x1d: return b->u16(i + 1);
        case 0x1a: case 0x1e: return b->u32(i + 1);
        case 0x1b: case 0x1f: return b->u64(i + 1);

        case 0x40: case 0x44: return b->u8 (i + 2);
        case 0x41: case 0x45: return b->u16(i + 3);
        case 0x42: case 0x46: return b->u32(i + 5);
        case 0x43: case 0x47: return b->u64(i + 9);

        case 0x50: return b->u16(i + 1) >> 1;
        case 0x51: return b->u32(i + 1) >> 2;
        case 0x52: return b->u64(i + 1) >> 3;
        case 0x53: return 0;

        default: throw voperation_error("len()", *this);
        } }
      } }


  uint64_t astride()        const { return atype().vsize(); }
  uint64_t asub(uint64_t i) const { return ibegin() + astride() * i; }

  tval atype() const
    { require_ibuf(); require_type(1ul << ARRAY);
      switch (b->u8(i))
      {
      case 0x44: return tval(*b, i + 3);
      case 0x45: return tval(*b, i + 5);
      case 0x46: return tval(*b, i + 9);
      case 0x47: return tval(*b, i + 17);
      default: throw internal_error("val atype()");
      } }


  operator float()  const { require_type(1ul << FLOAT32); return has_ibuf() ? b->f32(i + 1)                       : vf; }
  operator sym()    const { require_type(1ul << SYMBOL);  return has_ibuf() ? sym{b->u64(i + 1)}                  : vy; }
  operator pidfd()  const { require_type(1ul << PIDFD);   return has_ibuf() ? pidfd{b->u32(i + 1), b->u32(i + 5)} : vp; }
  operator bool()   const { require_type(1ul << BOOL);    return has_ibuf() ? b->u8(i) == 0x0d                    : !!vu; }

  operator double() const
    { require_type(1ul << FLOAT64 | 1ul << Θ);
      return type() == FLOAT64
        ? has_ibuf() ? b->f64(i + 1) : vd
        : static_cast<double>(static_cast<uint64_t>(*this))
        / static_cast<double>(std::numeric_limits<uint64_t>::max()); }

  operator uint64_t() const
    { if (type() == INT) throw voperation_error("i->u", *this);
      require_type(1ul << UINT | 1ul << Ρ | 1ul << Θ);
      if (!has_ibuf()) return vu;
      let x = b->u8(i);
      switch (x)
      {
      case 0x00: return b->u8 (i + 1);
      case 0x01: return b->u16(i + 1);
      case 0x02: return b->u32(i + 1);
      case 0x03: return b->u64(i + 1);
      default: throw internal_error("val u64");
      } }

  operator int64_t() const
    { if (type() == UINT) throw voperation_error("u->i", *this);
      require_type(1ul << INT);
      if (!has_ibuf()) return vi;
      let x = b->u8(i);
      if (x >= 0x80) return x - 0x80;
      switch (x)
      {
      case 0x04: return b->i8 (i + 1);
      case 0x05: return b->i16(i + 1);
      case 0x06: return b->i32(i + 1);
      case 0x07: return b->i64(i + 1);
      default: throw internal_error("val i64");
      } }

  operator std::string_view() const
    { require_type(1ul << UTF8 | 1ul << BYTES);
      return has_ibuf()
        ? std::string_view(reinterpret_cast<char const*>(mbegin()), mlen())
        : *reinterpret_cast<std::string_view const*>(vb); }


  hash h() const
    { let t = type();
      switch (t)
      {

#define ht(ct)                                                          \
        { let x = ce(static_cast<uint64_t>(static_cast<ct>(*this)));    \
          return xxh(&x, sizeof x, t); }
      case Ρ:
      case Θ:
      case UINT:    ht(uint64_t)
      case INT:     ht(int64_t)
      case SYMBOL:  ht(sym)
      case PIDFD:   ht(pidfd)
#undef ht

      case FLOAT64: { let x = ce(static_cast<double>(*this)); return xxh(&x, sizeof x, t); }
      case FLOAT32: { let x = ce(static_cast<float> (*this)); return xxh(&x, sizeof x, t); }

      case BOOL: return xxh(NULL, 0, static_cast<bool>(*this) ? 0x0d : 0x0c);

      case NULLTYPE:
      case Α:
      case Ω:
      case Ι:
      case Κ:
      case Τ: return xxh(NULL, 0, t);

      case ARRAY:
      case UTF8:
      case BYTES: return xxh(mbegin(), mend() - mbegin(), t);

      case TUPLE:
      { uint64_t h = xxh(NULL, 0, t);
        for (let &v : *this) h = xxc(v.h().h, h);
        return h; }

      default: throw voperation_error("h", *this);
      } }


  int compare(val const &v) const
    { let t1 =   type();
      let t2 = v.type();
      if (t1 != t2) throw binop_error("ct", *this, v);
      switch (t1)
      {

#define cmpblock(t)                                     \
        { let x1 = static_cast<t>(*this);               \
          let x2 = static_cast<t>(v);                   \
          return x1 > x2 ? 1 : x1 < x2 ? -1 : 0; }
      case Ρ:
      case Θ:
      case UINT:    cmpblock(uint64_t)
      case INT:     cmpblock(int64_t)
      case FLOAT32: cmpblock(float)
      case FLOAT64: cmpblock(double)
      case SYMBOL:  cmpblock(sym)
      case PIDFD:   cmpblock(pidfd)
      case BOOL:    cmpblock(bool)
#undef cmpblock

      case NULLTYPE:
      case Α:
      case Ω:
      case Κ:
      case Ι:
      case Τ: return 0;

      case UTF8:
      case BYTES:
      case ARRAY:
      { if (let tc = atype().compare(v.atype())) throw binop_error("cta", *this, v);
        let n1 =   mend() -   mbegin();
        let n2 = v.mend() - v.mbegin();
        if (let c = std::__memcmp(mbegin(), v.mbegin(), std::min(n1, n2))) return c;
        return n1 > n2 ? 1 : n1 < n2 ? -1 : 0; }

      case TUPLE:
      { bool m1 = false, m2 = false;
        for (it i1 = begin(), i2 = v.begin(), e1 = end(), e2 = v.end();
             m1 = i1 != e1, m2 = i2 != e2, m1 && m2;
             ++i1, ++i2)
          if (let c = (*i1).compare(*i2)) return c;
        return m1 ? 1 : m2 ? -1 : 0; }

      default: throw binop_error("nc", *this, v);
      } }

  bool operator< (val const &v) const { return this->compare(v) < 0; }
  bool operator> (val const &v) const { return this->compare(v) > 0; }
  bool operator<=(val const &v) const { return this->compare(v) <= 0; }
  bool operator>=(val const &v) const { return this->compare(v) >= 0; }

  bool operator==(val const &v) const { return type() == v.type() && this->compare(v) == 0; }
  bool operator!=(val const &v) const { return type() != v.type() || this->compare(v) != 0; }


  val &operator<<(val const &v)
    { require_type(1ul << TUPLE);
      if (has_ibuf()) throw voperation_error("val<< +ibuf", *this);
      vt->push_back(v);
      return *this; }


  val operator[](uint64_t i) const
    { return type() == ARRAY ? ap(i)
           : type() == TUPLE ? tp(i)
           : throw voperation_error("v[u64]", *this); }

  val operator[](val const &v) const
    { throw voperation_error("v[v] TODO", *this); }


  // NOTE: immediate vals are never arrays (since there isn't space to store
  // the array type + tag + vector). So this will be called only with an ibuf.
  val ap(uint64_t i) const { require_ibuf(); return val(atype(), *b, asub(i)); }

  // All functions below use hinting: hi or hk to indicate the thing that was
  // hinted, and h to indicate the byte offset (beyond mbegin()) to start
  // looking (for arrays, hi to indicate the element offset). In every case,
  // hi < i and hk < k (or for hashed, H[hk] < H[k]); decoding is forward-only,
  // so hints can't overshoot.
  val tp(uint64_t i, uint64_t hi = 0, uint64_t h = 0) const;


  struct kf_te { val const &operator()(val const &e)  { return e; } };
  struct kf_tk { val const  operator()(val const &kv) { return kv[0]; } };
  struct kf_tv { val const  operator()(val const &kv) { return kv[1]; } };

  struct kf_ae
  { ibuf const &b;
    tval const  t;
    uint64_t    s;
    uint64_t    o;
    kf_ae(val const &a) : b(*a.b), t(a.atype()), s(a.astride()), o(a.ibegin()) {}
    val const operator()(uint64_t i) const { return val(t, b, o + s*i); } };

  struct kf_ak
  { ibuf const &b;
    tval const  t;
    uint64_t    s;
    uint64_t    o;
    kf_ak(val const &a) : b(*a.b), t(a.atype()[0]), s(a.astride()), o(a.ibegin()) {}
    val const operator()(uint64_t i) const { return val(t, b, o + s*i); } };

  template <class KF> val io(val const &k) const;
  template <class KF> val ih(val const &k) const;

  template <class KF> val to(val const &k, val const &hk, uint64_t h  = 0) const;
  template <class KF> val th(val const &k, val const &hk, uint64_t h  = 0) const;
  template <class KF> val ao(val const &k, val const &hk, uint64_t hi = 0) const;
  template <class KF> val ah(val const &k, val const &hk, uint64_t hi = 0) const;

  template <class KF> val make_to() const;
  template <class KF> val make_th() const;
  template <class KF> val make_ao() const;
  template <class KF> val make_ah() const;
};


val const none(val::tagify(NONE), 0);

val const α(val::tagify(Α), 0);
val const ω(val::tagify(Ω), 0);
val const ι(val::tagify(Ι), 0);
val const κ(val::tagify(Κ), 0);
val const τ(val::tagify(Τ), 0);

inline val ρ(uint64_t x) { return val(val::tagify(Ρ), x); }
inline val θ(uint64_t x) { return val(val::tagify(Θ), x); }
inline val θ(double   x) { return val(val::tagify(Θ), static_cast<uint64_t>(x * static_cast<double>(std::numeric_limits<uint64_t>::max()))); }

inline val tuple(uint64_t n = 0) { let vt = new std::vector<val>; if (n) vt->reserve(n); return val(vt); }


namespace  // container search helper functions
{

template <class KF>
uint64_t binsearch(std::vector<val> const &vs,
                   val              const &k,
                   uint64_t                l = 0,
                   uint64_t                u = -1ul)
{
  KF kf;
  u = std::min(u, vs.size());
  while (u > l + 1)
  {
    let m = u + l >> 1;
    switch (kf(vs[m]).compare(k))
    {
    case  1: u = m; break;
    case -1: l = m; break;
    case  0: return m;
    }
  }
  return l | (kf(vs[l]).compare(k) ? 1ul << 63 : 0);
}


template <class KF>
uint64_t interpsearch(std::vector<val> const &vs,
                      val              const &k,
                      uint64_t                l = 0,
                      uint64_t                u = -1ul)
{
  KF kf;
  u = std::min(vs.size(), u);
  uint64_t hu = kf(vs[u - 1]).h();
  uint64_t hl = kf(vs[0]).h();
  let hk = k.h().h;

  // NOTE: interp-search for long distances where we're likely to choose
  // better splits; then delegate to binsearch to divide the final interval.
  while (u > l + 16)
  {
    let m  = std::min(u - 1, std::max(l + 1, l + (u - l) * (hk - hl) / (hu - hl)));
    let hm = kf(vs[m]).h();
    switch (hm.compare(hk))
    {
    case  1: u = m; hu = hm; break;
    case -1: l = m; hl = hm; break;
    case  0: return m;
    }
  }

  while (u > l + 1)
  {
    let m  = u + l >> 1;
    let hm = kf(vs[m]).h();
    switch (hm.compare(hk))
    {
    case  1: u = m; hu = hm; break;
    case -1: l = m; hl = hm; break;
    case  0: return m;
    }
  }

  return l | (kf(vs[l]).compare(k) ? 1ul << 63 : 0);
}


template <class KF>
val s_to(val      const &b,
         val      const &k,
         val      const &hk,
         uint64_t const h)
{
  if (hk.exists() && hk > k) throw internal_error("s_to hk>k");
  KF kf;
  if (!b.has_ibuf()) { let i = binsearch<KF>(*b.vt, k); return i & 1ul << 63 ? none : (*b.vt)[i]; }
  for (uint64_t o = b.ibegin() + h; o < b.iend(); o += b.b->len(o))
  {
    let v = val(*b.b, o);
    switch (k.compare(kf(v)))
    {
    case -1: break;
    case  0: return v;
    case  1: return none;
    }
  }
  return none;
}


template <class KF>
val s_th(val      const &b,
         val      const &k,
         val      const &hk,
         uint64_t const h)
{
  if (hk.exists() && hk > k) throw internal_error("s_th hk>k");
  KF kf;
  if (!b.has_ibuf()) { let i = interpsearch<KF>(*b.vt, k); return i & 1ul << 63 ? none : (*b.vt)[i]; }
  let kh = k.h().h;
  for (uint64_t o = b.ibegin() + h; o < b.iend(); o += b.b->len(o))
  {
    let v  = val(*b.b, o);
    let vh = kf(v).h().h;
    if (kh > vh)  return none;
    if (kh == vh) return v;
  }
  return none;
}


// TODO: s_ao and s_ah


}


inline val val::tp(uint64_t i, uint64_t const hi, uint64_t const h) const
{
  if (!has_ibuf()) return (*vt)[i];
  if (hi > i)      throw internal_error("hi>i");
  let      e = iend();
  uint64_t o = ibegin() + h;
  if (o >= e) throw decoding_error("tp0", *b, o);
  while (i-- > hi) if ((o += b->len(o)) >= e) throw decoding_error("tp", *b, o);
  return val(*b, o);
}

template <class KF> inline val val::to(val const &k, val const &hk, uint64_t const h) const { return s_to<KF>(*this, k, hk, h); }
template <class KF> inline val val::th(val const &k, val const &hk, uint64_t const h) const { return s_th<KF>(*this, k, hk, h); }

template <class KF> val val::make_to() const
{
  KF kf;
  let v = new std::vector<val>;
  v->reserve(len());
  for (let &x : *this) v->push_back(x);
  std::sort(v->begin(), v->end(),
            [=](val const &a, val const &b) { return kf(a) < kf(b); });
  return val(v);
}

template <class KF> val val::make_th() const
{
  struct hi { uint64_t h; uint64_t i; };
  static_assert(sizeof(hi) == sizeof(val));

  KF kf;
  let vh = new std::vector<hi>;
  vh->reserve(len());
  if (has_ibuf()) for (let &x : *this) vh->push_back(hi{kf(x).h(), x.i});
  else
  { uint64_t j = 0;
    for (let &x : *this) vh->push_back(hi{kf(x).h(), j++}); }

  std::sort(vh->begin(), vh->end(),
            [](hi const &a, hi const &b) { return a.h < b.h; });

  let vs = reinterpret_cast<std::vector<val>*>(vh);
  if (has_ibuf()) for (uint64_t i = 0; i < vs->size(); ++i) (*vs)[i] = val(*b, (*vh)[i].i);
  else            for (uint64_t i = 0; i < vs->size(); ++i) (*vs)[i] = (*vt)[(*vh)[i].i];

  return val(vs);
}


// TODO: make_ao and make_ah


inline oenc &tval::pack(oenc &o, val const &v) const
{
  switch (type())
  {
  case UINT:
  case INT:
    switch (typecode())
    {
    case 0x00: return o.u8 (cou8(v));
    case 0x01: return o.u16(cou16(v));
    case 0x02: return o.u32(cou32(v));
    case 0x03: return o.u64(v);
    case 0x04: return o.u8 (coi8(v));
    case 0x05: return o.u16(coi16(v));
    case 0x06: return o.u32(coi32(v));
    case 0x07: return o.u64(static_cast<int64_t>(v));
    default: throw internal_error("ipack");
    }
    break;

  case FLOAT32: return o.f32(static_cast<float>(v));
  case FLOAT64: return o.f64(static_cast<double>(v));
  case SYMBOL:  return o.u64(static_cast<uint64_t>(v));
  case PIDFD:   return o.u32(static_cast<pidfd>(v).pid).u32(static_cast<pidfd>(v).fd);

  case UTF8:
  case BYTES:
  { let l = v.mlen(), s = vsize();
    o.xs(v.mbegin(), std::min(l, s)).fill(0, l > s ? 0 : s - l);
    return o; }

  case TUPLE:
  { tval::it ti(begin()),   te(end());
    val::it  vi(v.begin()), ve(v.end());
    while (ti != te && vi != ve) (*ti).pack(o, *vi), ++ti, ++vi;
    if ((ti != te) != (vi != ve)) throw encoding_error("t vn>tn", *this, v);
    return o; }

  case ARRAY:
  { let      at = atype();
    uint64_t n  = len();
    val::it  vi(v.begin()), ve(v.end());
    while (n && vi != ve) at.pack(o, *vi), ++vi, n--;
    if (n || vi != ve) throw encoding_error("a vn>tn", *this, v);
    return o; }

  default: throw encoding_error("ns", *this, v);
  }
}


namespace  // bytecode encoding
{

// Tuple-emitting buffer
struct tenc : public oenc
{
  oenc     &b;
  uint64_t  si;   // starting position of this tuple within b
  uint64_t  l;    // current inner length of this tuple
  uint64_t  n;    // number of tuple elements
  int       lh;   // current header size in bytes

  tenc(oenc &b_, uint64_t n_) : b(b_), si(b.size()), l(0), n(n_), lh(0)
    { write_header(); }

  ~tenc() { write_header(); }

  int header_size()
    { let m = l | n;
      return !(m >> 8) && n < 8 ? 2
           : m >> 32            ? 17
           : m >> 16            ? 9
           : m >> 8             ? 5 : 3; }

  void ensure_hlen()
    { let h = header_size();
      if (h != lh) b.move(si + lh, si + h, l - si + lh);
      b.seek(si + (lh = h) + l); }

  void write_header()
    { b.seek(si);
      switch (lh)
      {
      case 2:  b.u8(0x48 + n).u8(l);     break;
      case 3:  b.u8(0x40).u8(l).u8(n);   break;
      case 5:  b.u8(0x41).u16(l).u16(n); break;
      case 9:  b.u8(0x42).u32(l).u32(n); break;
      case 17: b.u8(0x43).u64(l).u64(n); break;
      }
      b.seek(si + lh + l); }


  uint64_t size() const { return l + lh; }

  void fill(uint8_t c, uint64_t n)          { ensure_capacity(l += n); b.fill(c, n);   ensure_hlen(); }
  void push(uint8_t x)                      { ensure_capacity(++l);    b.u8(x);        ensure_hlen(); }
  void write(uint8_t const *xs, uint64_t n) { ensure_capacity(l += n); b.write(xs, n); ensure_hlen(); }

  void seek(uint64_t to) { b.seek(to + si + lh); }
  void move(uint64_t from, uint64_t to, uint64_t n)
    { if (to > from) ensure_capacity(l + (to - from));
      b.move(si + lh + from, si + lh + to, n); }

  void ensure_capacity(uint64_t l_) { b.ensure_capacity(si + l_); }
};


oenc &operator<<(oenc &o, val const &v)
{
  if (v.has_ibuf()) return o.xs(*v.b + v.i, v.len());

  switch (v.type())
  {
  case UINT:
  { uint64_t x = v;
    return x >> 32 ? o.u8(0x03).u64(x) : x >> 16 ? o.u8(0x02).u32(x)
         : x >>  8 ? o.u8(0x01).u16(x) :           o.u8(0x00).u8(x); }

  case INT:
  { int64_t x = v;
    if (x >= 0 && x < 128) return o.u8(0x80 + x);
    return x != x << 32 >> 32 ? o.u8(0x07).u64(x)
         : x != x << 48 >> 48 ? o.u8(0x06).u32(x)
         : x != x << 56 >> 56 ? o.u8(0x05).u16(x)
         :                      o.u8(0x04).u8(x); }

  case FLOAT32:  return o.u8(0x08).f32(v);
  case FLOAT64:  return o.u8(0x09).f64(v);
  case SYMBOL:   return o.u8(0x0a).u64(static_cast<sym>(v).h);
  case PIDFD:    return o.u8(0x0b).u32(static_cast<pidfd>(v).pid).u32(static_cast<pidfd>(v).fd);
  case BOOL:     return o.u8(static_cast<bool>(v) ? 0x0d : 0x0c);
  case NULLTYPE: return o.u8(0x0e);

  case Α: return o.u8(0x10);
  case Ω: return o.u8(0x11);
  case Ι: return o.u8(0x12);
  case Κ: return o.u8(0x13);
  case Τ: return o.u8(0x14);
  case Ρ: return o.u8(0x15).u64(v);
  case Θ: return o.u8(0x16).u64(v);

  case UTF8:
  { let l = v.len();
    return (l < 16  ? o.u8(0x20 + l) :
            l >> 32 ? o.u8(0x1b).u64(l) :
            l >> 16 ? o.u8(0x1a).u32(l) :
            l >> 8  ? o.u8(0x19).u16(l) : o.u8(0x18).u8(l))
      .xs(v.vb->data(), l); }

  case BYTES:
  { let l = v.len();
    return (l < 16  ? o.u8(0x30 + l) :
            l >> 32 ? o.u8(0x1f).u64(l) :
            l >> 16 ? o.u8(0x1e).u32(l) :
            l >> 8  ? o.u8(0x1d).u16(l) : o.u8(0x1c).u8(l))
      .xs(v.vb->data(), l); }

  case TUPLE: { tenc t(o, v.vt->size()); for (let &x : *v.vt) t << x; return o; }
  case ARRAY: throw internal_error("enc<<iarray");
  case INDEX: throw internal_error("enc<<idx no ibuf");

  case NONE:
  case BOGUS:
  default: throw voperation_error("enc<<v ns type", v);
  }
}

}


namespace  // val/tval << operators
{

std::ostream &operator<<(std::ostream &s, sym   const &y) { return s << 's' << reinterpret_cast<void*>(y.h); }
std::ostream &operator<<(std::ostream &s, hash  const &h) { return s << 'h' << reinterpret_cast<void*>(h.h); }
std::ostream &operator<<(std::ostream &s, pidfd const &p) { return s << "[p=" << p.pid << ",fd=" << p.fd << "]"; }

std::ostream &operator<<(std::ostream &s, val_type t)
{
  switch (t)
  {
  case UINT:     return s << "uint";
  case INT:      return s << "int";
  case FLOAT32:  return s << "f32";
  case FLOAT64:  return s << "f64";
  case SYMBOL:   return s << "sym";
  case PIDFD:    return s << "pidfd";
  case BOOL:     return s << "bool";
  case NULLTYPE: return s << "null";

  case Α:        return s << "α";
  case Ω:        return s << "ω";
  case Ι:        return s << "ι";
  case Κ:        return s << "κ";
  case Τ:        return s << "τ";
  case Ρ:        return s << "ρ";
  case Θ:        return s << "θ";

  case UTF8:     return s << "utf8";
  case BYTES:    return s << "bytes";
  case TUPLE:    return s << "tuple";
  case ARRAY:    return s << "array";
  case INDEX:    return s << "index";

  case BOGUS:    return s << "bogus";
  case NONE:     return s << "none";
  default:       return s << "??? " << static_cast<uint8_t>(t);
  }
}

std::ostream &operator<<(std::ostream &s, tval const &t)
{
  switch (t.type())
  {
  case UINT:    return s << "u" << t.vsize() * 8;
  case INT:     return s << "i" << t.vsize() * 8;
  case FLOAT32:
  case FLOAT64: return s << "f" << t.vsize() * 8;
  case SYMBOL:  return s << "s";
  case PIDFD:   return s << "p";

  case UTF8:    return s << "u[" << t.vsize() << "]";
  case BYTES:   return s << "b[" << t.vsize() << "]";
  case TUPLE:
  { bool first = true;
    s << "(";
    for (let &v : t)
    { if (first) first = false;
      else       s << ", ";
      s << v; }
    return s << ")"; }

  case ARRAY: return s << t.atype() << "[" << t.len() << "]";

  default: throw internal_error("ns typecode");
  }
}

std::ostream &operator<<(std::ostream &s, val const &v)
{
  switch (v.type())
  {
  case UINT:    return s << static_cast<uint64_t>(v);
  case INT:     return s << static_cast<int64_t>(v);
  case FLOAT32: return s << static_cast<float>(v);
  case FLOAT64: return s << static_cast<double>(v);
  case SYMBOL:  return s << static_cast<sym>(v);
  case PIDFD:   return s << static_cast<pidfd>(v);

  case Α: return s << "α";
  case Ω: return s << "ω";
  case Ι: return s << "ι";
  case Κ: return s << "κ";
  case Τ: return s << "τ";
  case Ρ: return s << "ρ(" << static_cast<uint64_t>(v) << ")";
  case Θ: return s << "θ(" << static_cast<double>(v) << ")";

  case UTF8:  return s << "u8[" << static_cast<std::string_view>(v) << "]";
  case BYTES: return s << "b["  << static_cast<std::string_view>(v) << "]";

  case TUPLE:
  { s << "(";
    bool first = true;
    for (let &x : v)
    { if (first) first = false;
      else       s << ", ";
      s << x; }
    return s << ")"; }

  case ARRAY:
  { s << "array<" << v.atype() << ">[" << v.len() << "][";
    bool first = true;
    for (uint64_t i = 0; i < v.len(); ++i)
    { if (first) first = false;
      else       s << ", ";
      s << val(v.atype(), *v.b, v.asub(i)); }
    return s << "]"; }

  case INDEX: return s << "index[]";

  case NONE:  return s << "none";
  case BOGUS: return s << "bogus";
  default:    return s << "???";
  }
}

std::ostream &operator<<(std::ostream &s, obuf const &o)
{
  return s.write(reinterpret_cast<char*>(o.b), o.i);
}

}


inline void utf9_init()
{
  init_lfn_tables();
  init_sfn_tables();
}


static_assert(sizeof(tval) == 24);
static_assert(sizeof(val)  == 16);


#undef BE
#undef LE

#undef let

}


template<> struct std::hash<tau::utf9::val>
{ uint64_t operator()(tau::utf9::val const &v) { return v.h(); } };

template<> struct std::hash<tau::utf9::tval>
{ uint64_t operator()(tau::utf9::tval const &t) { return t.h(); } };


#endif  // ifndef tau_utf9_h
