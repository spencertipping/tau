#ifndef tau_utf9_ibuf_h
#define tau_utf9_ibuf_h


#include <cstring>

#include "../types.hh"
#include "error-proto.hh"
#include "numerics.hh"

#include "../module/begin.hh"


namespace tau::utf9
{


// A bytecode decoder with fully-buffered and bounded source data.
struct ibuf
{
  u8c *xs;
  uN   l;
  bool owned;


  ibuf(u8c *xs_, uN l_, bool owned_ = false)
    : xs(xs_), l(l_), owned(owned_) {}

  ibuf(Il<int> xs_)
    : xs(new u8[xs_.size()]), l(xs_.size()), owned(true)
    { uN i = 0; for (let x : xs_) Cc<u8*>(xs)[i++] = cou8(x); }

  ibuf()              : xs(nullptr), l(0), owned(false) {}
  ibuf(ibuf const &b) : owned(false) { *this = b; }
  ibuf(ibuf &&b)      : owned(false) { *this = std::move(b); }

  ~ibuf() { if (owned) delete[] xs; }


  ibuf &operator=(ibuf const &b)
    { if (owned) delete[] xs;
      xs    = new u8[b.l];
      l     = b.l;
      owned = true;
      std::memcpy(Cc<u8*>(xs), b.xs, l);
      return *this; }

  ibuf &operator=(ibuf &&b)
    { if (owned) delete[] xs;
      xs    = b.xs;
      l     = b.l;
      owned = b.owned;
      b.xs    = nullptr;
      b.l     = 0;
      b.owned = false;
      return *this; }

  ibuf *copy() const { let r = new ibuf; *r = *this; return r; }


  bool has  (uN i) const { return i < l; }
  void check(uN i) const { if (!has(i)) throw_decoding_error("ibuf bounds", *this, i); }

  uN len  (uN i) const;
  uN tlen (uN i) const;
  uN tvlen(uN i) const;

  uN ctlen(uN i) const { check(i); let n = tlen(i); check(i + n - 1); return n; }


  u8c *data() const { return xs; }
  uN   size() const { return l; }


  u8c *operator+(uN i) const { return xs + i; }


  u8  U8  (uN i) const { return xs[i]; }
  u16 U16 (uN i) const { return ce(*Rc<u16c*>(xs + i)); }
  u32 U32 (uN i) const { return ce(*Rc<u32c*>(xs + i)); }
  u64 U64 (uN i) const { return ce(*Rc<u64c*>(xs + i)); }
  u8  cu8 (uN i) const { check(i);               return U8(i);  }
  u16 cu16(uN i) const { check(i); check(i + 1); return U16(i); }
  u32 cu32(uN i) const { check(i); check(i + 3); return U32(i); }
  u64 cu64(uN i) const { check(i); check(i + 7); return U64(i); }

  i8  I8  (uN i) const { return Rc<i8c*>(xs)[i]; }
  i16 I16 (uN i) const { return ce(*Rc<i16c*>(xs + i)); }
  i32 I32 (uN i) const { return ce(*Rc<i32c*>(xs + i)); }
  i64 I64 (uN i) const { return ce(*Rc<i64c*>(xs + i)); }
  u8  ci8 (uN i) const { check(i);               return I8(i);  }
  u16 ci16(uN i) const { check(i); check(i + 1); return I16(i); }
  u32 ci32(uN i) const { check(i); check(i + 3); return I32(i); }
  u64 ci64(uN i) const { check(i); check(i + 7); return I64(i); }

  f32 F32 (uN i) const { return ce(*Rc<f32c*>(xs + i)); }
  f64 F64 (uN i) const { return ce(*Rc<f64c*>(xs + i)); }
  f32 cf32(uN i) const { check(i); check(i + 3); return F32(i); }
  f64 cf64(uN i) const { check(i); check(i + 7); return F64(i); }
};


}


#include "../module/end.hh"

#endif
