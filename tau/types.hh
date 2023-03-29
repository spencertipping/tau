#ifndef τtypes_h
#define τtypes_h


#include <chrono>
#include <memory>
#include <type_traits>


#include "arch.hh"
#include "debug.hh"

#include "ctypes.hh"
#include "cptr.hh"

#include "begin.hh"


#define τuse_nonvolatile_sharedptr 0


namespace τ
{

using namespace std::literals;


#if !τuse_nonvolatile_sharedptr
  template<class... T> using Sp = std::shared_ptr<T...>;
  template<class... T> using Wp = std::weak_ptr<T...>;

  template<class U, class T> Sp<U> dpc(Sp<T> const &x) { return std::dynamic_pointer_cast<U>(x); }
#else
  template<class... T> using Sp = shared_ptr<T...>;
  template<class... T> using Wp = weak_ptr<T...>;

  template<class U, class T> Sp<U> dpc(Sp<T> const &x) { return x.template as<U>(); }
#endif


typedef std::chrono::steady_clock       Θc;
typedef std::chrono::nanoseconds        ΔΘ;
typedef std::chrono::time_point<Θc, ΔΘ> Θp;

// Definition of time quantum
ΔΘ constexpr Θq = 1ns;
static_assert(Θq.count() == 1);
static_assert(std::is_integral<ΔΘ::rep>::value);


// Construct next available key for map
template<class K, class V>
inline K ιi(K &c, M<K, V> const &m)
{
  while (m.contains(++c) || !c);
  return c;
}


struct ι  // ranged numeric iteration
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


}

#include "end.hh"


#endif
