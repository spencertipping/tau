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

template<class T> concept η0at = η0at_<T>::t != η0t::invalid;


typedef union { void *p; u64 u; i64 i; f64 f; bool b; } η0p;


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
#endif


}

#include "end.hh"


#endif
