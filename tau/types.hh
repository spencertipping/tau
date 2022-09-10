#ifndef tau_types_h
#define tau_types_h


#include <complex>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>


#include "arch.hh"
#include "debug.hh"


#include "begin.hh"

namespace tau
{


typedef std::uint8_t  u8;   typedef u8  const u8c;
typedef std::uint16_t u16;  typedef u16 const u16c;
typedef std::uint32_t u32;  typedef u32 const u32c;
typedef std::uint64_t u64;  typedef u64 const u64c;
typedef std::int8_t   i8;   typedef i8  const i8c;
typedef std::int16_t  i16;  typedef i16 const i16c;
typedef std::int32_t  i32;  typedef i32 const i32c;
typedef std::int64_t  i64;  typedef i64 const i64c;

typedef std::uint_fast8_t  uf8;   typedef uf8  const uf8c;
typedef std::uint_fast16_t uf16;  typedef uf16 const uf16c;
typedef std::uint_fast32_t uf32;  typedef uf32 const uf32c;
typedef std::uint_fast64_t uf64;  typedef uf64 const uf64c;
typedef std::int_fast8_t   if8;   typedef if8  const if8c;
typedef std::int_fast16_t  if16;  typedef if16 const if16c;
typedef std::int_fast32_t  if32;  typedef if32 const if32c;
typedef std::int_fast64_t  if64;  typedef if64 const if64c;

typedef double f64;  typedef f64 const f64c;
typedef float  f32;  typedef f32 const f32c;

typedef std::complex<f32> c32;  typedef c32 const c32c;
typedef std::complex<f64> c64;  typedef c64 const c64c;

typedef char ch;         // that's right, "char" is just too damn long
typedef char const chc;


#if tau_wordsize == 64
  typedef u64 uN;  typedef uN const uNc;
  typedef i64 iN;  typedef iN const iNc;
#elif tau_wordsize == 32
  typedef u32 uN;  typedef uN const uNc;
  typedef i32 iN;  typedef iN const iNc;
#else
# error unsupported word size
#endif


static_assert(sizeof(void*) == sizeof(uN));
static_assert(sizeof(void*) == sizeof(iN));

static_assert(sizeof(long long) == sizeof(u64));
static_assert(sizeof(f64)       == sizeof(u64));
static_assert(sizeof(f32)       == sizeof(u32));
static_assert(sizeof(c32)       == sizeof(f64));
static_assert(sizeof(char)      == sizeof(u8));

static_assert(sizeof(u8)  == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);

static_assert(sizeof(c64) == sizeof(f64) * 2);
static_assert(sizeof(c32) == sizeof(f32) * 2);

#if tau_wordsize == 64
  static_assert(sizeof(void*) == sizeof(u64));
#elif tau_wordsize == 32
  static_assert(sizeof(void*) == sizeof(u32));
#endif


template<class... T>       using D  = std::deque<T...>;
template<class... T>       using F  = std::function<T...>;
template<class... T>       using Il = std::initializer_list<T...>;
template<class T>          using H  = std::hash<T>;
template<class... T>       using M  = std::unordered_map<T...>;
template<class T>          using Nl = std::numeric_limits<T>;
template<class T, class U> using P  = std::pair<T, U>;
typedef             std::ostream O;
template<class T>          using Op = std::optional<T>;
template<class... T>       using Q  = std::queue<T...>;
template<class T>          using Rw = std::reference_wrapper<T>;
template<class... K>       using S  = std::unordered_set<K...>;
template<class... T>       using SP = std::shared_ptr<T...>;
template<class... X>       using T  = std::tuple<X...>;
template<class... T>       using V  = std::vector<T...>;
template<class... T>       using Va = std::variant<T...>;
template<class... T>       using Vi = typename V<T...>::const_iterator;

template<class T, class C = std::less<T>> using PQ = std::priority_queue<T, std::vector<T>, C>;


template<class T, class = void> struct iti_static : std::false_type {};
template<class T> struct iti_static
<T, std::void_t<decltype(std::begin(std::declval<T>())),
                decltype(std::end  (std::declval<T>()))>>
  : std::true_type {};

template<class T, class = void> struct iti_method : std::false_type {};
template<class T> struct iti_method
<T, std::void_t<decltype(std::declval<T>().begin()),
                decltype(std::declval<T>().end())>>
  : std::true_type {};

template<class T>
struct iti
{
  sletc v = iti_static<T>::value | iti_method<T>::value;
};



typedef uN      Λr;
typedef uN      λi;
typedef uN      ζi;
typedef uN      φi;
typedef F<Λr()> λf;

enum class λs  // lambda runnability state
{
  R,           // runnable
  S,           // stopped
  I,           // blocked on read from ζ
  O,           // blocked on write to ζ
  W,           // waiting for a λ
  Θ,           // waiting for a time
  ΦI,          // blocked on read from fd
  ΦO,          // blocked on write to fd
  φc,          // waiting for φc on a φ
  φx,          // waiting for φx on a φ
  Z,           // done (zombie)
};


template<class K, class V>
inline K ιi(K &c, M<K, V> const &m)
{
  while (m.contains(++c) || !c);
  return c;
}


typedef std::basic_string<u8>      B;
typedef std::basic_string_view<u8> Bv;
typedef std::string                St;
typedef std::string_view           Stv;


template<class T, class U> inline constexpr T Rc(U x) { return reinterpret_cast<T>(x); }
template<class T, class U> inline constexpr T Sc(U x) { return      static_cast<T>(x); }
template<class T, class U> inline constexpr T Cc(U x) { return       const_cast<T>(x); }


template<class T> inline constexpr u64 Su (T x) { return Sc<u64>(x); }
template<class T> inline constexpr i64 Si (T x) { return Sc<i64>(x); }
template<class T> inline constexpr uN  Sun(T x) { return Sc<uN> (x); }
template<class T> inline constexpr iN  Sin(T x) { return Sc<iN> (x); }
template<class T> inline constexpr f64 Sf (T x) { return Sc<f64>(x); }


struct ι
{
  uNc l;
  uNc h;

  ι(uN h_)        : l(0),  h(h_) {}
  ι(uN l_, uN h_) : l(l_), h(h_) {}

  struct it
  {
    uN x;
    uN   operator* ()            const { return x; }
    it  &operator++()                  { ++x; return *this; }
    bool operator==(it const &y) const { return x == y.x; }
  };

  it begin() const { return it{l}; }
  it end  () const { return it{h}; }
};


#if tau_debug_iostream
O &operator<<(O &s, λs t)
{
  switch (t)
  {
  case λs::R:  return s << "R";
  case λs::S:  return s << "S";
  case λs::I:  return s << "I";
  case λs::O:  return s << "O";
  case λs::W:  return s << "W";
  case λs::Θ:  return s << "Θ";
  case λs::ΦI: return s << "ΦI";
  case λs::ΦO: return s << "ΦO";
  case λs::φc: return s << "φc";
  case λs::φx: return s << "φx";
  case λs::Z:  return s << "Z";
  default:     return s << "BOGUS " << Su(t);
  }
}
#endif


}

#include "end.hh"


#endif
