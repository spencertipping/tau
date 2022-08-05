#ifndef tau_utf9_enc_h
#define tau_utf9_enc_h


#include "../types.hh"

#include "error-proto.hh"
#include "numerics.hh"
#include "obuf.hh"
#include "val.hh"

#include "../module/begin.hh"


namespace tau::utf9
{


// Tuple-emitting buffer
struct tenc : public oenc
{
  oenc &b;
  uN   si;   // starting position of this tuple within b
  uN   l;    // current inner length of this tuple
  uN   n;    // number of tuple elements
  int  lh;   // current header size in bytes

  tenc(oenc &b_, uN n_) : b(b_), si(b.size()), l(0), n(n_), lh(0)
    { write_header(); }

  ~tenc() { write_header(); }

  ibuf convert_to_ibuf() { return throw_internal_error<ibuf>("tenc->ibuf"); }

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


  uN size() const { return l + lh; }

  void fill(U8 c, uN n)          { ensure_capacity(l += n); b.fill(c, n);   ensure_hlen(); }
  void push(U8 x)                { ensure_capacity(++l);    b.u8(x);        ensure_hlen(); }
  void write(U8 const *xs, uN n) { ensure_capacity(l += n); b.write(xs, n); ensure_hlen(); }

  void seek(uN to) { b.seek(to + si + lh); }
  void move(uN from, uN to, uN n)
    { if (to > from) ensure_capacity(l + (to - from));
      b.move(si + lh + from, si + lh + to, n); }

  void ensure_capacity(uN l_) { b.ensure_capacity(si + l_); }
};


oenc &operator<<(oenc &o, val const &v)
{
  if (v.has_ibuf()) return o.xs(*v.b + v.i, v.len());

  switch (v.type())
  {
  case UINT8:  return o.u8(UINT8).u8(cou8(Sc<u64>(v)));
  case UINT16: return o.u8(UINT16).u16(cou16(Sc<u64>(v)));
  case UINT32: return o.u8(UINT32).u32(cou32(Sc<u64>(v)));
  case UINT64: return o.u8(UINT64).u64(Sc<u64>(v));

  case INT8:
  { let x = Sc<i64>(v);
    if (x >= 0 && x < 128) return o.u8(0x80 + x);
    else                   return o.u8(INT8).u8(coi8(x)); }

  case INT16: return o.u8(INT16).u16(coi16(Sc<i64>(v)));
  case INT32: return o.u8(INT32).u32(coi32(Sc<i64>(v)));
  case INT64: return o.u8(INT64).u64(Sc<i64>(v));

  case FLOAT32:  return o.u8(0x08).f32(v);
  case FLOAT64:  return o.u8(0x09).f64(v);
  case SYMBOL:   return o.u8(0x0a).u64(Sc<sym>(v).h);
  case PIDFD:    return o.u8(0x0b).u32(Sc<pidfd>(v).pid).u32(Sc<pidfd>(v).fd);
  case BOOL:     return o.u8(Sc<bool>(v) ? 0x0d : 0x0c);
  case NULLTYPE: return o.u8(0x0e);

  case UTF8:
  { let l = v.len();
    return (l < 16  ? o.u8(0x20 + l) :
            ou32(l) ? o.u8(0x1b).u64(l) :
            ou16(l) ? o.u8(0x1a).u32(l) :
            ou8(l)  ? o.u8(0x19).u16(l) : o.u8(0x18).u8(l))
      .xs(v.vb->data(), l); }

  case BYTES:
  { let l = v.len();
    return (l < 16  ? o.u8(0x30 + l) :
            ou32(l) ? o.u8(0x1f).u64(l) :
            ou16(l) ? o.u8(0x1e).u32(l) :
            ou8(l)  ? o.u8(0x1d).u16(l) : o.u8(0x1c).u8(l))
      .xs(v.vb->data(), l); }

  case TUPLE:
  { tenc t(o, v.vt->size());
    for (let &x : *v.vt) t << x;
    return o; }

  case ARRAY:
  { let n = v.len();
    let t = v.atype();
    let l = n * t.vsize() + t.tsize();

    if      (ou32(l)) o.u8(0x47).u64(l).u64(n);
    else if (ou16(l)) o.u8(0x46).u32(l).u32(n);
    else if (ou8(l))  o.u8(0x45).u16(l).u16(n);
    else              o.u8(0x44).u8(l) .u8(n);

    o.xs(t.b().xs + t.i, t.e - t.i);
    for (let &x : *v.vt) t.pack(o, x);
    return o; }

  case LIST: return throw_internal_error<oenc&>("TODO enc<<list");
  case SET:  return throw_internal_error<oenc&>("TODO enc<<set");
  case MAP:  return throw_internal_error<oenc&>("TODO enc<<map");

  case NONE:
  case BOGUS:
  default: return throw_vop_error<oenc&>("enc<<v ns type", v);
  }
}


}


#include "../module/end.hh"

#endif
