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


// A set of type codes packed into bits. Used to encode the type of each
// value within a η record, for fast function dispatching.
//
// The most-significant 4-bit value is the number of items in the set.
struct ηts final
{
  constexpr ηts(u64 x_)      : x(x_) {}
  constexpr ηts(Il<ηtype> l) : x(l.size() << 60)
    { uN i = 0;
      for (let y : l) x |= u64(y) << i++ * 4; }

  constexpr ηtype operator[](uN i) const { return ηtype(x >> (i * 4) & 0xf); }
  constexpr uN          size()     const { return x >> 60; }

  // Returns true if a function whose arguments are this set's types can be
  // applied to a value whose types are y.
  constexpr bool operator()(ηts y) const
    { let s = std::min(size(), y.size());
      return x & ~(-1ull << s * 4) == y.x & ~(-1ull << s * 4); }

  u64 x;
};


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


}

#include "end.hh"


#endif
