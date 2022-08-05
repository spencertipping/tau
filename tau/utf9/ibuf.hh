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
  U8 const * xs;
  uN         l;
  bool       owned;


  ibuf(u8 const *xs_, uN l_, bool owned_ = false)
    : xs(xs_), l(l_), owned(owned_) {}

  ibuf(Il<int> xs_)
    : xs(new U8[xs_.size()]), l(xs_.size()), owned(true)
    { U64 i = 0; for (let x : xs_) Cc<U8*>(xs)[i++] = cou8(x); }

  ibuf()              : xs(nullptr), l(0), owned(false) {}
  ibuf(ibuf const &b) : owned(false) { *this = b; }
  ibuf(ibuf &&b)      : owned(false) { *this = std::move(b); }

  ~ibuf() { if (owned) delete[] xs; }


  ibuf &operator=(ibuf const &b)
    { if (owned) delete[] xs;
      xs    = new U8[b.l];
      l     = b.l;
      owned = true;
      std::memcpy(Cc<U8*>(xs), b.xs, l);
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


  bool has  (uN i) const { return i < l; }
  void check(uN i) const { if (!has(i)) throw_decoding_error("ibuf bounds", *this, i); }

  uN len  (uN i) const;
  uN tlen (uN i) const;
  uN tvlen(uN i) const;

  uN ctlen(uN i) const { check(i); let n = tlen(i); check(i + n - 1); return n; }


  U8 const *data() const { return xs; }
  uN        size() const { return l; }


  U8 const *operator+(uN i) const { return xs + i; }


  U8  u8 (uN i) const { return xs[i]; }
  U16 u16(uN i) const { return ce(*Rc<u16c*>(xs + i)); }
  U32 u32(uN i) const { return ce(*Rc<u32c*>(xs + i)); }
  U64 u64(uN i) const { return ce(*Rc<u64c*>(xs + i)); }

  I8  i8 (uN i) const { return Rc<i8c*>(xs)[i]; }
  I16 i16(uN i) const { return ce(*Rc<i16c*>(xs + i)); }
  I32 i32(uN i) const { return ce(*Rc<i32c*>(xs + i)); }
  I64 i64(uN i) const { return ce(*Rc<i64c*>(xs + i)); }

  U8  ci8 (uN i) const { check(i);               return i8(i);  }
  U16 ci16(uN i) const { check(i); check(i + 1); return i16(i); }
  U32 ci32(uN i) const { check(i); check(i + 3); return i32(i); }
  U64 ci64(uN i) const { check(i); check(i + 7); return i64(i); }

  U8  cu8 (uN i) const { check(i);               return u8(i);  }
  U16 cu16(uN i) const { check(i); check(i + 1); return u16(i); }
  U32 cu32(uN i) const { check(i); check(i + 3); return u32(i); }
  U64 cu64(uN i) const { check(i); check(i + 7); return u64(i); }

  F32 f32(uN i) const { return ce(*Rc<f32c*>(xs + i)); }
  F64 f64(uN i) const { return ce(*Rc<f64c*>(xs + i)); }

  F32 cf32(uN i) const { check(i); check(i + 3); return f32(i); }
  F64 cf64(uN i) const { check(i); check(i + 7); return f64(i); }
};


}


#include "../module/end.hh"

#endif
