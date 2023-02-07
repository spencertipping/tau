#ifndef τηtypes_h
#define τηtypes_h


#include <type_traits>

#include "types.hh"
#include "begin.hh"

namespace τ
{


// η₀ frame type (not exported, just for internal logic):
// short, medium, long, disk
enum class η0ft : u8 { s, m, l, d };


// η₀ value type
enum class η0t : u8
{
  η0       = 0,
  signal   = 1,
  symbol   = 2,
  bytes    = 3,
  utf8     = 4,
  int_be   = 5,
  uint_be  = 6,
  float_be = 7,
  boolean  = 8,
  tuple    = 9,
  set      = 10,
  map      = 11,
  invalid  = 255
};


// η₀ type set
struct η0ts
{
  constexpr static u64 mask(Il<η0t> xs)
    { u64 m = 0;
      for (let x : xs) m |= Sc<u32>(1) << Sc<iN>(x);
      return m; }

  u64c m;

  constexpr η0ts(Il<η0t> xs) : m(mask(xs)) {}
  constexpr η0ts(u64 m_)     : m(m_)       {}

  constexpr bool operator[](η0t t) const
    { return Sc<uN>(t) < 64 && m & Sc<u64>(1) << Sc<uN>(t); }

  constexpr η0ts operator~()       const { return ~m; }
  constexpr η0ts operator|(η0ts x) const { return m | x.m; }
  constexpr η0ts operator&(η0ts x) const { return m & x.m; }
  constexpr η0ts operator^(η0ts x) const { return m ^ x.m; }
  constexpr η0ts operator-(η0ts x) const { return m & ~x.m; }
};


// η₁ signals
enum class η1sig : u8
{
  α = 1,
  κ = 2,
  ι = 3,
  τ = 4,
  ω = 5,
};


sletc η1tp = η0ts{η0t::η0, η0t::int_be, η0t::uint_be, η0t::float_be, η0t::boolean, η0t::signal};
sletc η1tb = η0ts{η0t::bytes, η0t::utf8};
sletc η1tc = η0ts{η0t::tuple, η0t::set, η0t::map};


O &operator<<(O &s, η0t t);
O &operator<<(O &s, η0ts x);
O &operator<<(O &s, η1sig x);


}

#include "end.hh"


#endif
