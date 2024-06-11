#ifndef σserver_Ωhm_h
#define σserver_Ωhm_h

#include <xxhash.h>

#include "Omega-types.hh"
#include "../begin.hh"

namespace σ
{


typedef XXH128_hash_t        ΩH;   // big hash
typedef decltype(ΩH::high64) Ωh;   // small/partial hash
struct                       Ωm;   // 64-bit packed meta (offset + size)
struct                       Ωhm;  // 128-bit hash/meta pair

typedef ΩH  const ΩHc;
typedef Ωh  const Ωhc;
typedef Ωm  const Ωmc;
typedef Ωhm const Ωhmc;


enum class Ωmv : τ::u8
{
  nonexistent = 0,
  deleted     = 1,
};


// Metadata: 64 bits, 0 = nonexistent, -1 = deleted, other values are packed.
struct Ωm final : Ωtypes
{
  constexpr Ωm() : m(0) {}
  constexpr Ωm(Ωmc&) = default;
  constexpr Ωm(u64 o, u64 s);
  constexpr Ωm(Ωmv v) : m(v == Ωmv::deleted ? u64(-1) : 0) {}
  Ωm(u8c *p) : m(*Rc<u64bc*>(p)) {}

  Ωm &operator=(Ωmc&) = default;
  τ::SO operator<=>(Ωmc &o) const
  { let oc = offset() <=> o.offset();
    return oc == τ::SO::equal ? size() <=> o.size() : oc; }

  explicit operator τ::u64() const { return m; }

  bool exists()     const { return m && ~m; }
  bool is_deleted() const { return !~m; }
  u64  offset()     const { return m & 0x0000'0fff'ffff'ffffull; }
  u64  size_error() const { return 1ull << std::max(0, int(m >> 59) + 1 - 16); }
  u64  size()       const
  { let e = int(m >> 59 & 0x1f);
    let r = m >> 44 & 0x7fff;
    return (1ull << e + 1)
         - (e >= 16 ? r << e + 1 - 16 : r >> 16 - (e + 1)); }

  void into(u8 *p) const { *Rc<u64b*>(p) = m; }


protected:
  τ::u64 m;  // exp:5 trim:15 offset:44
};


struct Ωhm final : Ωtypes
{
  constexpr Ωhm(Ωh h, u64 o, u64 s) : h(h), m(o, s) {}
  constexpr Ωhm(Ωh h, Ωm m)         : h(h), m(m)    {}
  constexpr Ωhm()                   : h(0), m()     {}
  constexpr Ωhm(Ωhmc&) = default;
  Ωhm(τ::u8c *p) : h(*Rc<τ::u64bc*>(p)), m(p + 8) {}

  Ωhm &operator=  (Ωhmc&) = default;
  auto operator<=>(Ωhmc &o) const { return h <=> o.h; }

  Ωh   hash()      const { return h; }
  Ωm   meta()      const { return m; }
  void into(u8 *p) const { *Rc<u64b*>(p) = h; m.into(p + 8); }


protected:
  Ωh h;  // high 64 bits of element hash
  Ωm m;
};


static_assert(sizeof(Ωhm) == 16);


ic Ωm::Ωm(τ::u64 o, τ::u64 s)
{
  using namespace τ;
  A(o < 1ull << 44, "Ωhm: o too large: " << o);
  A(s < 1ull << 32, "Ωhm: s too large: " << s);

  let om = o & 0x0000'0fff'ffff'ffffull;

  if (s == 0) m = om;
  else
  {
    u64 b = ubits(s);
    if (s == 1ull << b)
      // No trim required because we hit the exact size.
      //                         44 bits = 12+ 32------|
      m = b - 1 << 59 | om;
    else
    {
      // First, allocate an extra bit for s because it's larger than 1 << b.
      // Second, find the residue and quantize it into the trim bits, rounding
      // down to avoid over-trimming the size. The trim bits behave as a
      // floating-point mantissa, so we always align them to the top end of
      // their 15-bit range.
      ++b;
      let n = 1ull << b;
      let t = b >= 16 ? n - s >> b - 16
                      : n - s << 16 - b;
      m = b - 1 << 59 | (t & 0x7fff) << 44 | om;
    }
  }
}


inline τ::O &operator<<(τ::O &s, Ωm m)
{
  return s << "0x" << std::hex << m.offset()
           << "+" << std::dec << m.size()
           << "-" << m.size_error();
}

inline τ::O &operator<<(τ::O &s, Ωhmc &h)
{
  return s << "Ωhm[h=0x" << std::hex << h.hash()
           << " " << h.meta() << "]";
}


}

#include "../end.hh"

#endif
