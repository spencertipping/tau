#ifndef τtypes_h
#define τtypes_h


#include <chrono>
#include <type_traits>


#include "arch.hh"
#include "debug.hh"

#include "ctypes.hh"


#include "begin.hh"

namespace τ
{

using namespace std::literals;


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
