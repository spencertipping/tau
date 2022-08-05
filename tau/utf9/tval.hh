#ifndef tau_utf9_tval_h
#define tau_utf9_tval_h


#include "../types.hh"

#include "error-proto.hh"
#include "hash.hh"
#include "ibuf.hh"
#include "obuf.hh"
#include "typecode.hh"

#include "../module/begin.hh"


namespace tau::utf9
{


struct tbuf;


// A typecode beginning at a specific bytecode location
struct tval
{
  uN const tag;  // NOTE: contains ibuf* plus ownership bit
  uN const i;
  uN const e;

  static inline uN tagify(ibuf const * b_, bool own = false)
    { return Rc<uN>(b_) | (own ? 1 : 0); }


  tval(ibuf const &b_, uN i_ = 0, bool own = false)
    : tag(tagify(&b_, own)),
      i(i_),
      e(i + b().ctlen(i)) {}

  tval(tbuf const &b_, uN i_ = 0, bool own = false)
    : tag(tagify(Rc<ibuf const *>(&b_), own)),
      i(i_),
      e(i + b().ctlen(i)) {}

  tval(tbuf *b_)
    : tag(tagify(Rc<ibuf*>(b_), true)),
      i(0),
      e(b().ctlen(0)) {}

  tval(tval const &t) : tag(tagify(&t.b(), false)), i(t.i), e(t.e) {}

  ~tval() { if (tag & 1) delete Rc<ibuf*>(tag & ~1ull); }


  ibuf const &b() const { return *Rc<ibuf const*>(tag & ~1ull); }


  struct it
  {
    ibuf const * const b;
    uN                 i;

    tval operator* ()            const { return tval(*b, i); }
    it  &operator++()                  { i += b->tlen(i); return *this; }
    bool operator==(it const &x) const { return b == x.b && i == x.i; }
  };


  it begin() const
    { if (type() != TUPLE) return throw_top_error<it>("begin()", *this);
      if (typecode() >= 0x48 && typecode() <= 0x4f) return it{&b(), i + 2};
      switch (typecode())
      {
      case 0x40: return it{&b(), i + 3};
      case 0x41: return it{&b(), i + 5};
      case 0x42: return it{&b(), i + 9};
      case 0x43: return it{&b(), i + 17};
      default:   return throw_internal_error<it>("tval begin()");
      } }

  it end() const { return it{&b(), e}; }

  tval operator[](uN i) const { it i_ = begin(); while (i--) ++i_; return *i_; }
  uN offset_of(uN i) const
    { if (type() != TUPLE) return throw_top_error<u64>("offset_of()", *this);
      if (!i) return 0;
      --i;
      u64 s = 0;
      it i_ = begin();
      while (i--) s += (*i_).vsize(), ++i_;
      return s; }


  u8       typecode() const { return b().u8(i); }
  val_type type()     const { return bts[typecode()]; }
  uN       vsize()    const { return b().tvlen(i); }
  uN       tsize()    const { return b().tlen(i); }


  u64 len() const
    { if (type() != ARRAY && type() != TUPLE)       return throw_top_error<u64>("len()", *this);
      if (typecode() >= 0x48 && typecode() <= 0x4f) return typecode() - 0x48;
      switch (typecode())
      {
      case 0x40: case 0x44: return b().u8(i + 2);
      case 0x41: case 0x45: return b().u16(i + 3);
      case 0x42: case 0x46: return b().u32(i + 5);
      case 0x43: case 0x47: return b().u64(i + 9);
      default: return throw_internal_error<u64>("tval len()");
      } }

  tval atype() const
    { if (type() != ARRAY) return throw_top_error<tval>("atype()", *this);
      switch (typecode())
      {
      case 0x44: return tval(b(), i + 3);
      case 0x45: return tval(b(), i + 5);
      case 0x46: return tval(b(), i + 9);
      case 0x47: return tval(b(), i + 17);
      default: return throw_internal_error<tval>("tval atype()");
      } }


  // NOTE: this implementation requires that all typecodes are canonicalized
  // before being emitted to bytecode (otherwise comparisons will be unstable)
  int compare(tval const &t) const
    { let n1 =   tsize();
      let n2 = t.tsize();
      return n1 > n2 ? 1 : n1 < n2 ? -1 : std::memcmp(b() + i, t.b() + t.i, n1); }


  u64 h() const { return xxh(b() + i, e - i, 1); }

  // Encode a value without any type prefixes
  oenc &pack(oenc&, val const&) const;
};


struct tbuf : public ibuf
{
  tbuf()               : ibuf() {}
  tbuf(Il<int> xs_)    : ibuf(xs_) {}
  tbuf(ibuf const &b_) : ibuf(b_) {}
  tbuf(ibuf &&b_)      : ibuf(b_) {}

  operator tval() && { return tval(new tbuf(std::move(*this))); }
};


tbuf const tu8 {UINT8};
tbuf const tu16{UINT16};
tbuf const tu32{UINT32};
tbuf const tu64{UINT64};
tbuf const ti8 {INT8};
tbuf const ti16{INT16};
tbuf const ti32{INT32};
tbuf const ti64{INT64};
tbuf const tf32{FLOAT32};
tbuf const tf64{FLOAT64};

tbuf const tsym  {SYMBOL};
tbuf const tpidfd{PIDFD};


tbuf tutf8(uN l)
{
  return l < 16  ? tbuf{0x20 + cou8(l)}
       : ou32(l) ? obuf(9).u8(0x1b).u64(l).convert_to_ibuf()
       : ou16(l) ? obuf(5).u8(0x1a).u32(l).convert_to_ibuf()
       : ou8(l)  ? obuf(3).u8(0x19).u16(l).convert_to_ibuf()
       :           obuf(2).u8(0x18).u8(l).convert_to_ibuf();
}

tbuf tbytes(uN l)
{
  return l < 16  ? tbuf{0x30 + cou8(l)}
       : ou32(l) ? obuf(9).u8(0x1f).u64(l).convert_to_ibuf()
       : ou16(l) ? obuf(5).u8(0x1e).u32(l).convert_to_ibuf()
       : ou8(l)  ? obuf(3).u8(0x1d).u16(l).convert_to_ibuf()
       :           obuf(2).u8(0x1c).u8(l).convert_to_ibuf();
}

template<class T = Il<tbuf>>
tbuf ttuple(T const &xs)
{
  let n = xs.size();
  uN s = 0;
  for (let &x : xs) s += tval(x).vsize();

  obuf r(16);
  if      (ou32(s) || ou32(n)) r.u8(0x43).u64(s).u64(n);
  else if (ou16(s) || ou16(n)) r.u8(0x42).u32(s).u32(n);
  else if (ou8(s)  || ou8(n))  r.u8(0x41).u16(s).u16(n);
  else if (n < 8)              r.u8(0x48 + n).u8(s);
  else                         r.u8(0x40).u8(s).u8(n);

  for (let &x : xs) r << x;
  return r.convert_to_ibuf();
}

tbuf tarray(uN n, tbuf const &t)
{
  let s = tval(t).vsize() * n;
  return ou32(s) ? (obuf(16).u8(0x47).u64(s).u64(n) << t).convert_to_ibuf()
       : ou16(s) ? (obuf(16).u8(0x46).u32(s).u32(n) << t).convert_to_ibuf()
       : ou8(s)  ? (obuf(16).u8(0x45).u16(s).u16(n) << t).convert_to_ibuf()
       :           (obuf(16).u8(0x44).u8(s) .u8(n)  << t).convert_to_ibuf();
}


}


#include "../module/end.hh"

#endif
