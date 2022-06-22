#ifndef utf9_h
#define utf9_h


#include <bit>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include <byteswap.h>


namespace tau::utf9
{

#define let auto const


bool     ibuf_magic_init   = false;
uint64_t ibuf_ref_magic[2] = {0};


// Input buffer of bytes to be parsed as bytecode
struct ibuf
{
  uint64_t       magic[2];
  uint8_t const *xs;
  uint64_t       l;


  ibuf(uint8_t const *xs_, uint64_t l_) : xs(xs_), l(l_)
    { if (!ibuf_magic_init)
      { std::random_device                      rd;
        std::mt19937_64                         gen(rd());
        std::uniform_int_distribution<uint64_t> d;
        ibuf_ref_magic[0] = d(gen);
        ibuf_ref_magic[1] = d(gen);
        ibuf_magic_init   = true; }
      magic[0] = ibuf_ref_magic[0];
      magic[1] = ibuf_ref_magic[1]; }


  bool is_ibuf() const { return magic[0] == ibuf_ref_magic[0]
                             && magic[1] == ibuf_ref_magic[1]; }

  bool has  (uint64_t i) const { return i < l; }
  void check(uint64_t i) const { if (!has(i)) throw std::length_error("OOBE"); }

  uint64_t len  (uint64_t i) const;
  uint64_t tlen (uint64_t i) const;
  uint64_t tvlen(uint64_t i) const;


  uint8_t const *operator+(uint64_t i) const { return xs + i; }


  uint8_t  u8 (uint64_t i) const { return xs[i]; }
  uint16_t u16(uint64_t i) const
    { if constexpr (std::endian::native == std::endian::little)
                     return __bswap_16(*(uint16_t*)(xs + i));
      else if constexpr (std::endian::native == std::endian::big)
                          return *(uint16_t*)(xs + i); }

  uint32_t u32(uint64_t i) const
    { if constexpr (std::endian::native == std::endian::little)
                     return __bswap_32(*(uint32_t*)(xs + i));
      else if constexpr (std::endian::native == std::endian::big)
                          return *(uint32_t*)(xs + i); }

  uint64_t u64(uint64_t i) const
    { if constexpr (std::endian::native == std::endian::little)
                     return __bswap_64(*(uint64_t*)(xs + i));
      else if constexpr (std::endian::native == std::endian::big)
                          return *(uint64_t*)(xs + i); }


  int8_t  i8 (uint64_t i) const { return reinterpret_cast<int8_t const*>(xs)[i]; }
  int16_t i16(uint64_t i) const
    { if constexpr (std::endian::native == std::endian::little)
                     return __bswap_16(*(int16_t*)(xs + i));
      else if constexpr (std::endian::native == std::endian::big)
                          return *(int16_t*)(xs + i); }

  int32_t i32(uint64_t i) const
    { if constexpr (std::endian::native == std::endian::little)
                     return __bswap_32(*(int32_t*)(xs + i));
      else if constexpr (std::endian::native == std::endian::big)
                          return *(int32_t*)(xs + i); }

  int64_t i64(uint64_t i) const
    { if constexpr (std::endian::native == std::endian::little)
                     return __bswap_64(*(int64_t*)(xs + i));
      else if constexpr (std::endian::native == std::endian::big)
                          return *(int64_t*)(xs + i); }


  uint8_t  ci8 (uint64_t i) const { check(i);     return i8(i);  }
  uint16_t ci16(uint64_t i) const { check(i + 1); return i16(i); }
  uint32_t ci32(uint64_t i) const { check(i + 3); return i32(i); }
  uint64_t ci64(uint64_t i) const { check(i + 7); return i64(i); }

  uint8_t  cu8 (uint64_t i) const { check(i);     return u8(i);  }
  uint16_t cu16(uint64_t i) const { check(i + 1); return u16(i); }
  uint32_t cu32(uint64_t i) const { check(i + 3); return u32(i); }
  uint64_t cu64(uint64_t i) const { check(i + 7); return u64(i); }

  float  f32(uint64_t i) const { let x = u32(i); return *(float *)(&x); }
  double f64(uint64_t i) const { let x = u64(i); return *(double*)(&x); }
};


enum val_type : uint8_t
{
  REF     = 0,
  UINT    = 1,
  INT     = 2,
  FLOAT32 = 3,
  FLOAT64 = 4,
  SYMBOL  = 5,
  PIDFD   = 6,
  TAU     = 7,

  ALPHA   = 8,
  OMEGA   = 9,
  IOTA    = 10,
  KAPPA   = 11,

  UTF8    = 12,
  BYTES   = 13,
  TUPLE   = 14,
  ARRAY   = 15,
  INDEX   = 16,

  BOGUS   = 255,
};


namespace
{

val_type const bts[256] =
{
  // 0x00-0x0f
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  FLOAT32, FLOAT64, SYMBOL, PIDFD,
  BOGUS, BOGUS, BOGUS, BOGUS,

  // 0x10-0x1f
  ALPHA, OMEGA, IOTA, KAPPA,
  TAU, TAU, BOGUS, BOGUS,
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


let bogus_pf =
  [](ibuf const &b, uint64_t i) -> uint8_t const*
    { throw std::invalid_argument("bogus"); };

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


#undef pf

}


struct sym
{
  uint64_t h;

  sym(std::string const &s) : h(std::hash<std::string>{}(s)) {}
  sym(uint64_t h_)          : h(h_)                          {}
};


struct hash
{
  uint64_t h;
};


struct pidfd
{
  uint32_t pid;
  uint32_t fd;

  bool operator<(pidfd const &v) const { return pid < v.pid || pid == v.pid && fd < v.fd; }
  bool operator>(pidfd const &v) const { return pid > v.pid || pid == v.pid && fd > v.fd; }
};


struct tau
{
  uint64_t t;
};


struct val
{
  union
  { ibuf const * b;
    uint64_t     tag; };

  union
  { uint64_t i;
    uint64_t vu;
    int64_t  vi;
    double   vd;
    float    vf;
    pidfd    vp;
    sym      vs;
    tau      vt; };


  val(ibuf const &b_, uint64_t i_) : b(&b_),       i(i_)   {}
  val(uint64_t vu_)                : tag(UINT),    vu(vu_) {}
  val(int64_t vi_)                 : tag(INT),     vi(vi_) {}
  val(double vd_)                  : tag(FLOAT64), vd(vd_) {}
  val(float vf_)                   : tag(FLOAT32), vf(vf_) {}
  val(sym vs_)                     : tag(SYMBOL),  vs(vs_) {}
  val(tau vt_)                     : tag(TAU),     vt(vt_) {}
  val(pidfd vp_)                   : tag(PIDFD),   vp(vp_) {}


  bool     has_ibuf() const { return tag >= 256 && b->is_ibuf(); }
  val_type type()     const { return has_ibuf() ? bts[b->cu8(i)] : static_cast<val_type>(tag); }


  val list() const
    { uint64_t j = i; while (bts[b->cu8(j)] == INDEX) j += b->len(j); return j; }

  uint8_t const *begin() const { return sfns[b->cu8(i)](*b, i); }
  uint8_t const *end()   const { return *b + b->len(i); }
  val            first() const { return val(*b, begin() - b->xs); }
  val            next()  const { return val(*b, i + b->len(i)); }


  uint64_t len() const
    { if (!has_ibuf()) throw std::invalid_argument("len");
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
      case 0x41: case 0x45: return b->u16(i + 2);
      case 0x42: case 0x46: return b->u32(i + 2);
      case 0x43: case 0x47: return b->u64(i + 2);

      case 0x50: return b->u16(i + 1) >> 1;
      case 0x51: return b->u32(i + 1) >> 2;
      case 0x52: return b->u64(i + 1) >> 3;
      case 0x53: return 0;

      default: throw std::invalid_argument("len");
      } }


  operator uint64_t() const
    { if (type() != UINT) throw std::invalid_argument("uint64");
      if (!has_ibuf())    return vu;
      let x = b->u8(i);
      switch (x)
      {
      case 0x00: return b->u8 (i + 1);
      case 0x01: return b->u16(i + 1);
      case 0x02: return b->u32(i + 1);
      case 0x03: return b->u64(i + 1);
      default: throw std::invalid_argument("uint64");
      } }

  operator int64_t() const
    { if (type() != INT) throw std::invalid_argument("int64");
      if (!has_ibuf())   return vi;
      let x = b->u8(i);
      if (x >= 0x80) return x - 0x80;
      switch (x)
      {
      case 0x04: return b->i8 (i + 1);
      case 0x05: return b->i16(i + 1);
      case 0x06: return b->i32(i + 1);
      case 0x07: return b->i64(i + 1);
      default: throw std::invalid_argument("int64");
      } }

  operator float() const
    { if (type() != FLOAT32) throw std::invalid_argument("float32");
      return has_ibuf() ? b->f32(i + 1) : vf; }

  operator double() const
    { if (type() != FLOAT64) throw std::invalid_argument("float64");
      return has_ibuf() ? b->f64(i + 1) : vd; }

  operator sym() const
    { if (type() != SYMBOL) throw std::invalid_argument("sym");
      return has_ibuf() ? sym{b->u64(i + 1)} : vs; }

  operator pidfd() const
    { if (type() != PIDFD) throw std::invalid_argument("pidfd");
      return has_ibuf() ? pidfd{b->u32(i + 1), b->u32(i + 5)} : vp; }

  operator tau() const
    { if (type() != TAU) throw std::invalid_argument("(tau)");
      return has_ibuf() ?
        b->u8(i) == 0x14 ? tau{0} : tau{b->u64(i + 1)} : vt; }


  uint64_t hash() const
    { let t = type();
      switch (t)
      {
      case UINT:
      case INT:
      case FLOAT64:
      case FLOAT32:
      case SYMBOL:
      case PIDFD:
      case TAU:   return murmur3a(static_cast<uint64_t>(*this));

      case ALPHA:
      case OMEGA:
      case IOTA:
      case KAPPA: return murmur3a(t);

      case ARRAY:
      case UTF8:
      case BYTES: return std::hash<T>{}(begin(), end());

      case TUPLE:
      { uint64_t h = 0;
        for (val v : *this) h = hash(h, v.hash());
        return h; }

      case INDEX: return list().hash();

      default: throw std::invalid_argument("hash");
      } }


  int compare(val const &v) const
    { let t1 =   type();
      let t2 = v.type();
      if (t1 != t2) return (int) t1 - (int) t2;
      switch (t1)
      {

#define cmpblock(t)                                     \
        { let x1 = static_cast<t>(*this);               \
          let x2 = static_cast<t>(v);                   \
          return x1 > x2 ? 1 : x1 < x2 ? -1 : 0; }

      case UINT:    cmpblock(uint64_t)
      case INT:     cmpblock(int64_t)
      case FLOAT32: cmpblock(float)
      case FLOAT64: cmpblock(double)
      case PIDFD:   cmpblock(pidfd)

      case SYMBOL:
      case TAU:     cmpblock(uint64_t)

#undef cmpblock

      case UTF8:
      case BYTES:
      case ARRAY:
      { let n1 =   end() -   begin();
        let n2 = v.end() - v.begin();
        let c  = std::__memcmp(begin(), v.begin(), std::min(n1, n2));
        return c ? c : n1 > n2 ? 1 : n1 < n2 ? -1 : 0; }

      case TUPLE:
      { let      n1 =   len();
        let      n2 = v.len();
        uint64_t n  = std::min(n1, n2);
        val      v1 =   first();
        val      v2 = v.first();
        while (n--)
        { let c = v1.compare(v2);
          v1 = v1.next();
          v2 = v2.next();
          if (c) return c; }
        return n1 > n2 ? 1 : n1 < n2 ? -1 : 0; }

      case INDEX: return list().compare(v.list());

      default: throw std::invalid_argument("cmp");
      } }
};


// TODO: what kind of dispatch structure do we want for the val
// operation space?


typedef uint64_t(*lfn)(ibuf const &, uint64_t);


namespace
{

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
let str8_lf   = lf(2 + b.cu8 (i + 1));
let str16_lf  = lf(3 + b.cu16(i + 1));
let str32_lf  = lf(5 + b.cu32(i + 1));
let str64_lf  = lf(9 + b.cu64(i + 1));

let list8_lf  = lf(3  + b.cu8 (i + 1));
let list16_lf = lf(5  + b.cu16(i + 1));
let list32_lf = lf(9  + b.cu32(i + 1));
let list64_lf = lf(17 + b.cu64(i + 1));

let u8_tvlf  = lf(b.cu8 (i + 1));
let u16_tvlf = lf(b.cu16(i + 1));
let u32_tvlf = lf(b.cu32(i + 1));
let u64_tvlf = lf(b.cu64(i + 1));

let fixutf8_lf   = lf(1 + (b.cu8(i) - 0x20));
let fixbytes_lf  = lf(1 + (b.cu8(i) - 0x30));
let fixtuple8_lf = lf(1 + b.cu8(i + 1));
let fixint_lf    = l1;

let idx16_lf = lf(1 + 2 + b.cu16(i + 1));
let idx32_lf = lf(1 + 4 + b.cu32(i + 1));
let idx64_lf = lf(1 + 8 + b.cu64(i + 1));

let bogus_lf =
  [](ibuf const &b, uint64_t i) -> uint64_t
  { throw std::invalid_argument("bogus"); };


// Typecode length functions
inline uint64_t tuple_tl(ibuf const &b, uint64_t i, uint64_t n)
{
  uint64_t l = 0;
  while (n--) l += b.tlen(i + l);
  return l;
}

let fixtuple_tlf = lf(2 + tuple_tl(b, i + 2, b.cu8(i) - 0x48));


// Typecode value length functions
let fixutf8_tvlf   = lf(b.cu8(i) - 0x20);
let fixbytes_tvlf  = lf(b.cu8(i) - 0x30);
let fixtuple8_tvlf = u8_tvlf;


// Bytecode decode tables
lfn const lfns[256] =   // 2kiB dcache footprint (common case)
{
  // 0x00 - 0x0f
  l2, l3, l5, l9,
  l2, l3, l5, l9,
  l5, l9, l9, l9,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x10 - 0x1f
  l1, l1, l1, l1,
  l1, l9, bogus_lf, bogus_lf,

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
  lf(1 + 2 + 1 + b.cu16(i + 1)),
  lf(1 + 4 + 1 + b.cu32(i + 1)),
  lf(1 + 8 + 1 + b.cu64(i + 1)),
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

lfn const tlfns[256] =  // 640B dcache footprint (worst-case)
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
  lf(3  + tuple_tl(b, i + 3,  b.cu8 (i + 2))),
  lf(5  + tuple_tl(b, i + 5,  b.cu16(i + 3))),
  lf(9  + tuple_tl(b, i + 9,  b.cu32(i + 5))),
  lf(17 + tuple_tl(b, i + 17, b.cu64(i + 9))),

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

lfn const tvlfns[256] = // 640B dcache footprint (worst-case)
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


#undef lf

}


inline uint64_t ibuf::len  (uint64_t i) const { check(i); return lfns  [xs[i]](*this, i); }
inline uint64_t ibuf::tlen (uint64_t i) const { check(i); return tlfns [xs[i]](*this, i); }
inline uint64_t ibuf::tvlen(uint64_t i) const { check(i); return tvlfns[xs[i]](*this, i); }


#undef let

}


#endif  // ifndef utf9_h
