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


// Primitive states for η₀ values
typedef union { void *p; u64 u; i64 i; f64 f; bool b; } η0p;

// Types that correspond to primitives
sletc η0tp = η0ts{η0t::η0, η0t::int_be, η0t::uint_be, η0t::float_be, η0t::boolean};


template<class T> struct η0at_ { sletc t = η0t::invalid; };
template<> struct η0at_<u8>    { sletc t = η0t::uint_be; };
template<> struct η0at_<u16>   { sletc t = η0t::uint_be; };
template<> struct η0at_<u32>   { sletc t = η0t::uint_be; };
template<> struct η0at_<u64>   { sletc t = η0t::uint_be; };
template<> struct η0at_<i8>    { sletc t = η0t::int_be; };
template<> struct η0at_<i16>   { sletc t = η0t::int_be; };
template<> struct η0at_<i32>   { sletc t = η0t::int_be; };
template<> struct η0at_<i64>   { sletc t = η0t::int_be; };
template<> struct η0at_<f32>   { sletc t = η0t::float_be; };
template<> struct η0at_<f64>   { sletc t = η0t::float_be; };
template<> struct η0at_<bool>  { sletc t = η0t::boolean; };

// η₀ assignable-from types (and from primitive)
template<class T> concept η0at  = η0at_<T>::t != η0t::invalid;
template<class T> concept η0atp = η0tp[η0at_<T>::t];


#if τdebug_iostream
O &operator<<(O &s, η0t t)
{
  switch (t)
  {
  case η0t::η0:       return s << "η₀";
  case η0t::signal:   return s << "sig";
  case η0t::symbol:   return s << "sym";
  case η0t::bytes:    return s << "b[]";
  case η0t::utf8:     return s << "utf8";
  case η0t::int_be:   return s << "int";
  case η0t::uint_be:  return s << "uint";
  case η0t::float_be: return s << "float";
  case η0t::boolean:  return s << "bool";
  case η0t::tuple:    return s << "()";
  case η0t::set:      return s << "set";
  case η0t::map:      return s << "map";
  case η0t::invalid:  return s << "invalid";
  default:            return s << "t" << Sc<uN>(t);
  }
}

O &operator<<(O &s, η0ts x)
{
  u64 m = x.m;
  s << "{";
  for (iN i = 0; m >> i; ++i)
  {
    if (m >> i & 1)
    {
      s << Sc<η0t>(i);
      if (m >> i + 1) s << ",";
    }
  }
  return s << "}";
}
#endif


}

#include "end.hh"


#endif
