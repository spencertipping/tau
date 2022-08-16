#ifndef tau_types_h
#define tau_types_h


#define tau_defines_types 1


#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <memory>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>


#include "arch.hh"
#include "debug.hh"


// enable this to prevent uint8_t, size_t etc from being used
// within tau code
#define TAU_ERASE_CSTDINT_TYPES 1


#include "module/begin.hh"

namespace tau
{


typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef double   f64;
typedef float    f32;

typedef u8  const u8c;
typedef u16 const u16c;
typedef u32 const u32c;
typedef u64 const u64c;
typedef i8  const i8c;
typedef i16 const i16c;
typedef i32 const i32c;
typedef i64 const i64c;
typedef f64 const f64c;
typedef f32 const f32c;


typedef std::ostream O;


template<class T>          using D  = std::deque<T>;
template<class T>          using F  = std::function<T>;
template<class T>          using Il = std::initializer_list<T>;
template<class T>          using H  = std::hash<T>;
template<class K, class V> using M  = std::unordered_map<K, V>;
template<class T>          using Nl = std::numeric_limits<T>;
template<class T, class U> using P  = std::pair<T, U>;
template<class T, class C> using PQ = std::priority_queue<T, std::vector<T>, C>;
template<class T>          using Q  = std::queue<T>;
template<class K>          using S  = std::unordered_set<K>;
template<class T>          using SP = std::shared_ptr<T>;
template<class T>          using V  = std::vector<T>;
template<class T>          using Vi = typename std::vector<T>::const_iterator;


typedef std::basic_string<uint8_t>      B;
typedef std::basic_string_view<uint8_t> Bv;


typedef size_t  uN;  // native int size
typedef ssize_t iN;

typedef uN const uNc;
typedef iN const iNc;


template<class T, class U> inline constexpr T Rc(U x) { return reinterpret_cast<T>(x); }
template<class T, class U> inline constexpr T Sc(U x) { return      static_cast<T>(x); }
template<class T, class U> inline constexpr T Cc(U x) { return       const_cast<T>(x); }


template<class T> inline constexpr u64 Su(T x) { return Sc<u64>(x); }
template<class T> inline constexpr i64 Si(T x) { return Sc<i64>(x); }
template<class T> inline constexpr f64 Sf(T x) { return Sc<f64>(x); }


#if TAU_ERASE_CSTDINT_TYPES
  typedef void uint8_t;
  typedef void uint16_t;
  typedef void uint32_t;
  typedef void uint64_t;
  typedef void int8_t;
  typedef void int16_t;
  typedef void int32_t;
  typedef void int64_t;

  typedef void size_t;
  typedef void ssize_t;
#endif


static_assert(sizeof(long long) == sizeof(u64));
static_assert(sizeof(f64)       == sizeof(u64));
static_assert(sizeof(f32)       == sizeof(u32));
static_assert(sizeof(char)      == sizeof(u8));


#if tau_wordsize == 64
  static_assert(sizeof(void*) == sizeof(u64));
#elif tau_wordsize == 32
  static_assert(sizeof(void*) == sizeof(u32));
#endif

static_assert(sizeof(void*) == sizeof(uN));


}

#include "module/end.hh"


#endif
