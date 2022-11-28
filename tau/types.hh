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


typedef uN      Λr;  // NOTE: this value is never used
typedef uN      λi;
typedef uN      λt;
typedef uN      ζi;
typedef uN      φi;
typedef uN      γi;
typedef F<Λr()> λf;


typedef std::chrono::steady_clock       Θc;
typedef std::chrono::nanoseconds        ΔΘ;
typedef std::chrono::time_point<Θc, ΔΘ> Θp;

// Definition of time quantum
ΔΘ constexpr Θq = 1ns;
static_assert(Θq.count() == 1);
static_assert(std::is_integral<ΔΘ::rep>::value);


enum class λs  // lambda runnability state
{
  R,           // runnable
  S,           // stopped
  Y,           // explicitly yielded
  I,           // blocked on read from ζ
  O,           // blocked on write to ζ
  W,           // waiting for a λ
  Θ,           // waiting for a time
  ΦI,          // blocked on read from fd
  ΦO,          // blocked on write to fd
  φc,          // waiting for φc on a φ
  φx,          // waiting for φx on a φ
};


template<class K, class V>
inline K ιi(K &c, M<K, V> const &m)
{
  while (m.contains(++c) || !c);
  return c;
}


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


#if τdebug_iostream
O &operator<<(O &s, λs t)
{
  switch (t)
  {
  case λs::R:  return s << "R";
  case λs::S:  return s << "S";
  case λs::Y:  return s << "Y";
  case λs::I:  return s << "I";
  case λs::O:  return s << "O";
  case λs::W:  return s << "W";
  case λs::Θ:  return s << "Θ";
  case λs::ΦI: return s << "ΦI";
  case λs::ΦO: return s << "ΦO";
  case λs::φc: return s << "φc";
  case λs::φx: return s << "φx";
  default:     return s << "BOGUS " << Sc<uN>(t);
  }
}
#endif


}

#include "end.hh"


#endif
