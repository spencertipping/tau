#ifndef tau_utf9_enc_h
#define tau_utf9_enc_h


#include "../types.hh"
#include "../util/numerics.hh"

#include "error-proto.hh"
#include "obuf.hh"
#include "val.hh"


#include "../module/begin.hh"

namespace tau::utf9
{


using namespace tau::util::numerics;


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
           : ou32(m)            ? 17
           : ou16(m)            ? 9
           : ou8(m)             ? 5 : 3; }

  void ensure_hlen()
    { let h = header_size();
      if (h != lh) b.move(si + lh, si + h, l - si + lh);
      b.seek(si + (lh = h) + l); }

  void write_header()
    { b.seek(si);
      switch (lh)
      {
      case 2:  b.U8(0x48 + n).U8(l);     break;
      case 3:  b.U8(0x40).U8(l).U8(n);   break;
      case 5:  b.U8(0x41).U16(l).U16(n); break;
      case 9:  b.U8(0x42).U32(l).U32(n); break;
      case 17: b.U8(0x43).U64(l).U64(n); break;
      }
      b.seek(si + lh + l); }


  uN size() const { return l + lh; }

  void fill(u8 c, uN n)     { ensure_capacity(l += n); b.fill(c, n);   ensure_hlen(); }
  void push(u8 x)           { ensure_capacity(++l);    b.U8(x);        ensure_hlen(); }
  void write(u8c *xs, uN n) { ensure_capacity(l += n); b.write(xs, n); ensure_hlen(); }

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
  case UINT8:  return o.U8(UINT8).U8(cou8(Sc<u64>(v)));
  case UINT16: return o.U8(UINT16).U16(cou16(Sc<u64>(v)));
  case UINT32: return o.U8(UINT32).U32(cou32(Sc<u64>(v)));
  case UINT64: return o.U8(UINT64).U64(Sc<u64>(v));

  case INT8:
  { let x = Sc<i64>(v);
    if (x >= 0 && x < 128) return o.U8(0x80 + x);
    else                   return o.U8(INT8).U8(coi8(x)); }

  case INT16: return o.U8(INT16).U16(coi16(Sc<i64>(v)));
  case INT32: return o.U8(INT32).U32(coi32(Sc<i64>(v)));
  case INT64: return o.U8(INT64).U64(Sc<i64>(v));

  case UINT:
  { let x = Sc<u64>(v);
    return ou32(x) ? o.U8(UINT64).U64(x)
         : ou16(x) ? o.U8(UINT32).U32(x)
         : ou8(x)  ? o.U8(UINT16).U16(x)
         :           o.U8(UINT8).U8(x); }

  case INT:
  { let x = Sc<i64>(v);
    if (x >= 0 && x < 128) return o.U8(0x80 + x);
    return oi32(x) ? o.U8(INT64).U64(x)
         : oi16(x) ? o.U8(INT32).U32(x)
         : oi8(x)  ? o.U8(INT16).U16(x)
         :           o.U8(INT8).U8(x); }

  case FLOAT32:  return o.U8(0x08).F32(v);
  case FLOAT64:  return o.U8(0x09).F64(v);
  case SYMBOL:
  { let h = Sc<sha256>(v);
    return o.U8(0x0a).U64(h.xs[0]).U64(h.xs[1]).U64(h.xs[2]).U64(h.xs[3]); }

  case PIDFD:    return o.U8(0x0b).U32(Sc<pidfd>(v).pid).U32(Sc<pidfd>(v).fd);
  case BOOL:     return o.U8(Sc<bool>(v) ? 0x0d : 0x0c);
  case NULLTYPE: return o.U8(0x0e);

  case UTF8:
  { let l = v.len();
    return (l < 16  ? o.U8(0x20 + l) :
            ou32(l) ? o.U8(0x1b).U64(l) :
            ou16(l) ? o.U8(0x1a).U32(l) :
            ou8(l)  ? o.U8(0x19).U16(l) : o.U8(0x18).U8(l))
      .xs(v.vb->data(), l); }

  case BYTES:
  { let l = v.len();
    return (l < 16  ? o.U8(0x30 + l) :
            ou32(l) ? o.U8(0x1f).U64(l) :
            ou16(l) ? o.U8(0x1e).U32(l) :
            ou8(l)  ? o.U8(0x1d).U16(l) : o.U8(0x1c).U8(l))
      .xs(v.vb->data(), l); }

  case TUPLE:
  { tenc t(o, v.vt->size());
    for (let &x : *v.vt) t << x;
    return o; }

  case ARRAY:
  { let n = v.len();
    let t = v.atype();
    let l = n * t.vsize() + t.tsize();

    if      (ou32(l)) o.U8(0x47).U64(l).U64(n);
    else if (ou16(l)) o.U8(0x46).U32(l).U32(n);
    else if (ou8(l))  o.U8(0x45).U16(l).U16(n);
    else              o.U8(0x44).U8(l) .U8(n);

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
