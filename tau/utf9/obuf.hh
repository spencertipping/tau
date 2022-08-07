#ifndef tau_utf9_obuf_h
#define tau_utf9_obuf_h


#include "../types.hh"
#include "../util/numerics.hh"

#include "ibuf.hh"


#include "../module/begin.hh"

namespace tau::utf9
{


using namespace tau::util::numerics;


// A bytecode encoder with no specified storage backend
struct oenc
{
  // Target size reduction of index wrt collection; that is,
  // sizeof(idx) <= sizeof(thing) >> INDEX_SIZE_BITS
  static const constexpr u8 INDEX_SIZE_BITS = 3;


  virtual ~oenc() {};

  virtual void ensure_capacity(uN l_) = 0;
  virtual void push(u8 x) = 0;
  virtual void write(u8c *xs, uN n) = 0;
  virtual void move(uN from, uN to, uN n) = 0;
  virtual void seek(uN to) = 0;
  virtual uN   size() const = 0;
  virtual void fill(u8 c, uN n) = 0;
  virtual ibuf convert_to_ibuf() = 0;


  virtual u8 list_pos_bits(uN n, uN l) const
    { if (n <= 16) return 64;
      let  s = n << bu(l);
      auto r = 0;
      while (s >> r >= l >> INDEX_SIZE_BITS) ++r;
      return r; }

  virtual u8 set_rcp_bits(uN n, uN l) const
    { if (n <= 16) return 64;

      // Return a value such that s * (~0ull >> r) <= l >> INDEX_SIZE_BITS
      // -- or, solving for r:
      //
      // ~0ull >> r <= (l >> INDEX_SIZE_BITS) / s
      // ~0ull >> r <= (l >> ISB + b)
      //          r >= 64 - (ilog(l) - ISB - b)
      let  b = bu(l);
      auto r = 64 - (ilog(l) - INDEX_SIZE_BITS - b);
      while (~0ull >> r >= l >> INDEX_SIZE_BITS + b) ++r;
      return r; }

  virtual u8 map_rcp_bits(uN n, uN l) const
    { return set_rcp_bits(n, l); }


  oenc &U8 (u8  x) { push(x); return *this; }
  oenc &U16(u16 x) { U8 (x >> 8);  return U8 (x & 0xff); }
  oenc &U32(u32 x) { U16(x >> 16); return U16(x & 0xffff); }
  oenc &U64(u64 x) { U32(x >> 32); return U32(x & 0xffffffff); }
  oenc &F32(f32 x) { union { f32 xf; u32 xi; }; xf = x; return U32(xi); }
  oenc &F64(f64 x) { union { f64 xd; u64 xi; }; xd = x; return U64(xi); }

  oenc &xs(u8c *xs, uN n) { write(xs, n); return *this; }

  oenc &operator<<(u8  x) { return U8(x);  }
  oenc &operator<<(u16 x) { return U16(x); }
  oenc &operator<<(u32 x) { return U32(x); }
  oenc &operator<<(u64 x) { return U64(x); }
  oenc &operator<<(f32 x) { return F32(x); }
  oenc &operator<<(f64 x) { return F64(x); }
  oenc &operator<<(ibuf const &b) { return xs(b.xs, b.l); }

  template <class T>
  oenc &operator<<(std::basic_string<T> x)
    { static_assert(sizeof x.data()[0] == 1);
      write(Rc<u8*>(x.data()), x.size());
      return *this; }

  template <class T>
  oenc &operator<<(std::basic_string_view<T> x)
    { static_assert(sizeof x.data()[0] == 1);
      write(Rc<u8*>(x.data()), x.size());
      return *this; }
};


// A fully buffered bytecode encoder
struct obuf : public virtual oenc
{
  u8 *b;  // buffer data (will be moved)
  uN  l;  // allocated size
  uN  i;  // position of next byte to be written

  obuf()     : b(nullptr),   l(0), i(0) {}
  obuf(uN c) : b(new u8[c]), l(c), i(0) {}

  ~obuf() { if (b) delete[] b; }


  void clear()
    { if (b) delete[] b;
      b = nullptr;
      i = l = 0; }


  ibuf convert_to_ibuf()
    { let b_ = b;
      let i_ = i;
      b = nullptr;
      l = i = 0;
      return ibuf{b_, i_, true}; }


  u8 *data()     const { return b; }
  uN  size()     const { return i; }
  uN  capacity() const { return l; }

  void push(u8 x)           { ensure_capacity(i + 1); b[i++] = x; }
  void write(u8c *xs, uN n) { ensure_capacity(i + n); memcpy(b + i, xs, n); i += n; }
  void fill(u8 c, uN n)     { ensure_capacity(i + n); memset(b + i,  c, n); i += n; }

  void seek(uN to) { i = to; }
  void move(uN from, uN to, uN n)
    { if (to > from) ensure_capacity(l + (to - from));
      std::memmove(b + to, b + from, n); }

  void ensure_capacity(uN l_)
    { if (b == nullptr) { b = new u8[1024]; l = 1024; }
      else if (l_ > l)
      { uN nl = l; while (nl < l_) nl <<= 1;
        u8 *b_ = new u8[nl];
        memcpy(b_, b, i);
        delete[] b;
        b = b_;
        l = nl; } }
};


}

#include "../module/end.hh"


#endif
