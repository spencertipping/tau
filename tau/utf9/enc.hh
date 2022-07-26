#ifndef tau_utf9_enc_h
#define tau_utf9_enc_h


#include <cstdint>

#include "obuf.hh"
#include "val.hh"

#include "../module/begin.hh"


namespace tau::utf9
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

  ibuf convert_to_ibuf() { throw internal_error("tenc cti"); }

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


#include "../module/end.hh"

#endif
