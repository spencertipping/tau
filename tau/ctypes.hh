#ifndef τctypes_h
#define τctypes_h


#include <array>
#include <compare>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <regex>
#include <set>
#include <span>
#include <stack>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>


// NOTE: this boost module is vendored into dep/
#include <boost/endian.hpp>


#include "arch.hh"


#include "begin.hh"

namespace τ
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

typedef char                ch;  // that's right, "char" is just too damn long
typedef char const          chc;
typedef unsigned char       uch;
typedef unsigned char const uchc;


typedef boost::endian::big_int8_t  i8b;
typedef boost::endian::big_int16_t i16b;
typedef boost::endian::big_int32_t i32b;
typedef boost::endian::big_int64_t i64b;

typedef boost::endian::big_uint8_t  u8b;
typedef boost::endian::big_uint16_t u16b;
typedef boost::endian::big_uint32_t u32b;
typedef boost::endian::big_uint64_t u64b;

typedef boost::endian::big_float32_t f32b;
typedef boost::endian::big_float64_t f64b;


typedef i8b  const i8bc;
typedef i16b const i16bc;
typedef i32b const i32bc;
typedef i64b const i64bc;

typedef u8b  const u8bc;
typedef u16b const u16bc;
typedef u32b const u32bc;
typedef u64b const u64bc;

typedef f32b const f32bc;
typedef f64b const f64bc;


// NOTE: "size_t", "int", and "long" don't coerce easily from sized ints,
// even when those types match size and signedness; as a result, we define
// fixed-width ints for "native width" and define size_t separately
#if τwordsize == 64
  typedef u64 uN;  typedef uN const uNc;
  typedef i64 iN;  typedef iN const iNc;
#elif τwordsize == 32
  typedef u32 uN;  typedef uN const uNc;
  typedef i32 iN;  typedef iN const iNc;
#else
# error unsupported word size
#endif


letc uNs = sizeof(uN);


typedef size_t  uS;  typedef uS const uSc;
typedef ssize_t iS;  typedef iS const iSc;


static_assert(sizeof(void*) == sizeof(uN));
static_assert(sizeof(void*) == sizeof(iN));
static_assert(sizeof(void*) == sizeof(uS));
static_assert(sizeof(void*) == sizeof(iS));

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

#if τwordsize == 64
  static_assert(sizeof(void*) == sizeof(u64));
#elif τwordsize == 32
  static_assert(sizeof(void*) == sizeof(u32));
#endif


// Standardize our representation of FDs across systems
typedef int fd_t;


template<class T, uN S>    using Ar = std::array<T, S>;
template<class... T>       using D  = std::deque<T...>;
template<class... T>       using F  = std::function<T...>;
template<class... T>       using Il = std::initializer_list<T...>;
template<class T>          using H  = std::hash<T>;
template<class T>          using Lg = std::lock_guard<T>;
template<class... T>       using M  = std::unordered_map<T...>;
template<class... T>       using Mc = std::unordered_map<T...> const;
template<class... T>       using Mo = std::map<T...>;
typedef               std::mutex Mu;
template<class T>          using Nl = std::numeric_limits<T>;
typedef             std::ostream O;
template<class T>          using Op = std::optional<T>;
template<class T, class U> using P  = std::pair<T, U>;
template<class... T>       using Q  = std::queue<T...>;
typedef               std::regex Re;
typedef              std::smatch Rsm;
typedef     std::sregex_iterator Rsi;
template<class... K>       using S  = std::unordered_set<K...>;
template<class... T>       using Sk = std::stack<T...>;
template<class... T>       using Sn = std::span<T...>;
template<class... K>       using So = std::set<K...>;
typedef        std::stringstream Ss;
template<class... X>       using T  = std::tuple<X...>;
typedef              std::thread Th;
template<class... T>       using Up = std::unique_ptr<T...>;
template<class... T>       using V  = std::vector<T...>;
template<class... T>       using Vc = std::vector<T...> const;
template<class... T>       using Va = std::variant<T...>;
template<class... T>       using Vi = typename V<T...>::const_iterator;

template<class T, class C = std::less<T>> using PQ = std::priority_queue<T, std::vector<T>, C>;


template<class T>                  using De = std::decay_t<T>;
template<bool X, class T>          using If = std::enable_if_t<X, T>;
template<bool X, class T, class F> using Co = std::conditional_t<X, T, F>;
template<uN... Xs>                 using Is = std::index_sequence<Xs...>;

template<class T>          ic bool Isi = std::is_integral_v<T>;
template<class T>          ic bool Isf = std::is_fundamental_v<T>;
template<class T, class U> ic bool Eq  = std::is_same_v<T, U>;


typedef std::strong_ordering  SO;
typedef std::weak_ordering    WO;
typedef std::partial_ordering PO;


typedef std::basic_string<u8>      B;
typedef std::basic_string_view<u8> Bv;
typedef std::string                St;
typedef std::string_view           Stv;

typedef B   const Bc;
typedef St  const Stc;
typedef Bv  const Bvc;
typedef Stv const Stvc;


inline auto ma(uN    s) { return std::malloc(s); }
inline void fr(void *x) { std::free(x); }


template<class... Xs>
ic decltype(auto) bi(Xs && ...xs) { return std::bind(std::forward<Xs>(xs)...); }

template<class T>
ic decltype(auto) mo(T &&x) { return std::move(x); }

template<class T> ic T&& fo(T &&x) { return Sc<T&&>(x); }
template<class T> ic T&& fo(T  &x) { return Sc<T&&>(x); }

template<class T, class U>
ic decltype(auto) mp(T &&a, U &&b) { return std::make_pair(std::forward<T>(a), std::forward<U>(b)); }

template<class T, class U>
ic decltype(auto) flip(P<T, U> const &p) { return mp(std::get<1>(p), std::get<0>(p)); }


}


#include "end.hh"


#endif
