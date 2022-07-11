#ifndef tau_utf9_ibuf_h
#define tau_utf9_ibuf_h


#include <cstdint>
#include <cstring>

#include "errors.hh"
#include "numerics.hh"

#include "../begin.hh"


namespace tau::utf9
{


// A bytecode decoder with fully-buffered and bounded source data.
struct ibuf
{
  uint8_t const * xs;
  uint64_t        l;
  bool            owned;

  ibuf(uint8_t const *xs_, uint64_t l_, bool owned_ = false)
    : xs(xs_), l(l_), owned(owned_) {}

  ibuf(std::initializer_list<int> xs_)
    : xs(new uint8_t[xs_.size()]), l(xs_.size()), owned(true)
    { uint64_t i = 0; for (let x : xs_) const_cast<uint8_t*>(xs)[i++] = cou8(x); }

  ibuf(ibuf const &b) : xs(new uint8_t[b.l]), l(b.l), owned(true)
    { std::memcpy(const_cast<uint8_t*>(xs), b.xs, l); }

  ~ibuf() { if (owned) delete[] xs; }


  bool has  (uint64_t i) const { return i < l; }
  void check(uint64_t i) const { if (!has(i)) throw decoding_error("ibuf bounds", *this, i); }

  uint64_t len  (uint64_t i) const;
  uint64_t tlen (uint64_t i) const;
  uint64_t tvlen(uint64_t i) const;

  uint64_t ctlen(uint64_t i) const
    { check(i);
      let n = tlen(i);
      check(i + n - 1);
      return n; }


  uint8_t const *data() const { return xs; }
  size_t         size() const { return l; }


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


}


#include "../end.hh"

#endif
