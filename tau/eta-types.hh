#ifndef τηtypes_h
#define τηtypes_h

#include "ctypes.hh"
#include "types.hh"

#include "begin.hh"

namespace τ
{


enum class ηsig : u8
{
  α = 0,
  κ = 1,
  ι = 2,
  τ = 3,
  ω = 4,
  Ω = 5,
  ρ = 6,
  θ = 7,

  max = 7,
};


enum class ηatom : u8
{
  ηfalse   = 0,
  ηtrue    = 1,
  bool_max = 1,
  ηnull    = 2,
  max      = 2,
};


enum class ηtype : u8
{
  sig      = 0,
  n_int    = 1,
  n_float  = 2,
  string   = 3,
  atom     = 4,
  η        = 5,
  name     = 6,
  res7     = 7,   // reserved
  int8s    = 8,
  int16s   = 9,
  int32s   = 10,
  int64s   = 11,
  float32s = 12,
  float64s = 13,
  res14    = 14,  // reserved
  res15    = 15,  // reserved
  invalid  = 15,  // reserved for illegal values
  max      = 15,
};


// Alternative type used to mark the fact that this thing is meant to be
// a name rather than a string. Otherwise we'd have ambiguity at the C++
// type-mapping level (e.g. in ηauto).
struct ηname
{
  St x;
};


// An unordered set of types encoded as a 16-bit mask.
struct ηts final
{
  constexpr ηts(u16 x_)      : x(x_) {}
  constexpr ηts(Il<ηtype> l) : x(0) { for (let y : l) x |= 1 << u8(y); }

  constexpr bool operator[](ηtype t) const { return x & 1 << u8(t); }
  constexpr bool      empty()        const { return x == 0; }

  constexpr ηts operator|(ηts y) const { return u16(x |  y.x); }
  constexpr ηts operator&(ηts y) const { return u16(x &  y.x); }
  constexpr ηts operator^(ηts y) const { return u16(x ^  y.x); }
  constexpr ηts operator-(ηts y) const { return u16(x & ~y.x); }
  constexpr ηts operator+(ηts y) const { return *this | y; }

  u16 x;
};


// An ordered list of types packed into a 64-bit int, with the top four
// bits reserved for the number of elements in the list.
//
// Elements are stored at fixed positions, so the first element of the list
// always occupies the lowest four bits. This provides fast prefix matches.
struct ηtl final
{
  constexpr ηtl(u64 x_)      : x(x_) {}
  constexpr ηtl(Il<ηtype> l) : x(-1ull)
    { A(l.size() < 16, "ηts overflow: " << l.size() << " elements");
      for (let y : l) x <<= 4, x |= u64(y);
      x = x & 0x0fffffffffffffffull | u64(l.size()) << 60; }

  constexpr ηtype operator[](uN i) const { return ηtype(x >> i * 4 & 0xf); }
  constexpr uN          size()     const { return x >> 60; }

  // Returns true if a function whose arguments are this set's types can be
  // applied to a value whose types are y.
  constexpr bool operator()(ηtl y) const
    { return x & ~(-1ull << size() * 4) == y.x & ~(-1ull << size() * 4); }

  u64 x;
};


// Least-upper-bound: use this to determine the return type of most binary
// operators, e.g. a + b.
ηtype ηlub(ηtype a, ηtype b);


// Number of bytes used to encode the size, or 0 if the size is directly
// inlined into the control byte
inline uN ηsb(uN s)
{
  return s <= 12 ? 0 : s < 269 ? 1 : s < 65805 ? 2 : 4;
}


// Writes a control byte + size and returns the number of bytes written
inline uN ηcb(Sn<u8> b, ηtype t, uN s)
{
  let sb = ηsb(s);
  let cb = Sc<u8>(t) << 4 | (!sb ? s : sb == 1 ? 0x0d : sb == 2 ? 0x0e : 0x0f);
  A(b.size_bytes() >= sb + 1, "ηcb overflow; |b| = " << b.size_bytes() << ", needed " << sb + 1);
  A(s < 65805 + Nl<u32>::max(), "ηcb size too large: " << s);

  b[0] = cb;

  switch (sb)
  {
  case 0: break;
  case 1: b[1] = s - 13; break;
  case 2:
  { let n = s - 269;
    b[1] = n >> 8 & 0xff;
    b[2] = n      & 0xff;
    break; }
  case 4:
  { let n = s - 65805;
    b[1] = n >> 24 & 0xff;
    b[2] = n >> 16 & 0xff;
    b[3] = n >> 8  & 0xff;
    b[4] = n       & 0xff;
    break; }
  }

  return 1 + sb;
}


O &operator<<(O&, ηsig);
O &operator<<(O&, ηatom);
O &operator<<(O&, ηtype);
O &operator<<(O&, ηts);
O &operator<<(O&, ηtl);
O &operator<<(O&, ηname const&);


}

#include "end.hh"


#endif
