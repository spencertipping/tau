#ifndef tau_utf9_obuf_h
#define tau_utf9_obuf_h


#include <cstdint>

#include "ibuf.hh"

#include "../module/begin.hh"


namespace tau::utf9
{


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
  virtual ibuf     convert_to_ibuf() = 0;

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

  obuf() {}
  obuf(uint64_t c) : b(new uint8_t[c]), l(c), i(0) {}

  virtual ~obuf() { if (b) delete[] b; }


  ibuf convert_to_ibuf()
    { let b_ = b;
      let i_ = i;
      b = nullptr;
      l = i = 0;
      return ibuf{b_, i_, true}; }


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


}


#include "../module/end.hh"

#endif
