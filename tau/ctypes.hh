#ifndef τctypes_h
#define τctypes_h


#include <array>
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
#include <stack>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>


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


// NOTE: "size_t", "int", and "long" don't coerce easily from sized ints,
// even when those types match size and signedness
#if τwordsize == 64
  typedef u64 uN;  typedef uN const uNc;
  typedef i64 iN;  typedef iN const iNc;
#elif τwordsize == 32
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
template<class... K>       using S  = std::unordered_set<K...>;
template<class... K>       using So = std::set<K...>;
template<class... T>       using Sk = std::stack<T...>;
template<class... T>       using Sp = std::shared_ptr<T...>;
typedef        std::stringstream Ss;
template<class... X>       using T  = std::tuple<X...>;
typedef              std::thread Th;
template<class... T>       using Up = std::unique_ptr<T...>;
template<class... T>       using V  = std::vector<T...>;
template<class... T>       using Vc = std::vector<T...> const;
template<class... T>       using Va = std::variant<T...>;
template<class... T>       using Vi = typename V<T...>::const_iterator;
template<class... T>       using Wp = std::weak_ptr<T...>;

template<class T, class C = std::less<T>> using PQ = std::priority_queue<T, std::vector<T>, C>;


template<class T, class U>
ic auto mp(T &&a, U &&b) { return std::make_pair(std::forward<T>(a), std::forward<U>(b)); }

template<class T>
ic auto ms(T &&a) { return std::make_shared<T>(std::forward<T>(a)); }


template<class U, class T>
ic Sp<U> dpc(Sp<T> x) { return std::dynamic_pointer_cast<U>(x); }


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


typedef std::basic_string<u8>      B;
typedef std::basic_string_view<u8> Bv;
typedef std::string                St;
typedef std::string_view           Stv;

typedef B   const Bc;
typedef St  const Stc;
typedef Bv  const Bvc;
typedef Stv const Stvc;


}


#include "end.hh"


#endif
