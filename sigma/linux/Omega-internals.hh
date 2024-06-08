#ifndef σserver_Ωinternals_h
#define σserver_Ωinternals_h

#include <xxhash.h>

#include "../prof.hh"
#include "../begin.hh"

namespace σ
{


typedef XXH128_hash_t        ΩH;  // big hash
typedef decltype(ΩH::high64) Ωh;  // small/partial hash

struct Ωhm;
typedef Ωhm const Ωhmc;


struct Ωhm   // 16-byte hash/meta pair, with packed meta
{
  Ωhm() = default;
  Ωhm(τ::u8c *p) : h(*Rc<τ::u64bc*>(p)), m(*Rc<τ::u64bc*>(p + 8)) {}
  Ωhm(Ωh h, τ::u64 o, τ::u64 s);
  Ωhm &operator=(Ωhmc &o) = default;

  τ::SO operator<=>(Ωhmc &o) const { return h <=> o.h; }

  Ωh     hash()       const { return h; }
  τ::u64 offset()     const { return m & 0x0000'0fff'ffff'ffffull; }
  τ::u64 size_error() const { return 1ull << std::max(0, int(m >> 59) + 1 - 16); }
  τ::u64 size()       const
  { let e = int(m >> 59 & 0x1f);
    let r = m >> 44 & 0x7fff;
    return (1ull << e + 1)
         - (e >= 16 ? r << e + 1 - 16 : r >> 16 - (e + 1)); }

  void into(τ::u8 *p) const
  { *Rc<τ::u64b*>(p)     = h;
    *Rc<τ::u64b*>(p + 8) = m; }


protected:
  Ωh     h;  // high 64 bits of element hash
  τ::u64 m;  // exp:5 trim:15 offset:44
};


inline Ωhm::Ωhm(Ωh h, τ::u64 o, τ::u64 s)
  : h(h)
{
  A(o < (1ull << 44), "Ωhm: o too large: " << o);
  A(s < (1ull << 32), "Ωhm: s too large: " << s);

  let om = o & 0x0000'0fff'ffff'ffffull;

  if (s == 0) m = om;
  else
  {
    τ::u64 b = τ::ubits(s);
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

  A(o == offset(), "Ωhm o != offset: " << o << " != " << offset());
  A(size() >= s,   "Ωhm size < s: " << size() << " < " << s);
  A(s <= 2 || size() <= s * (1.0 + 1.0/32768),
    "Ωhm size > +0.003%: " << size() << " > 1.003% * " << s);
  A(s <= 2 || size() - s <= size_error(),
    "Ωhm size - s > size_error: " << size() << " - " << s << " > " << size_error());
}


inline τ::O &operator<<(τ::O &s, Ωhmc &h)
{
  return s << "Ωhm[h=0x" << std::hex << h.hash()
           << " o=0x" << h.offset()
           << " s=" << std::dec << h.size() << "-" << h.size_error() << "]";
}


}

#include "../end.hh"

#endif
