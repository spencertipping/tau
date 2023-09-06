#ifndef τsha_h
#define τsha_h

#include <picosha2.h>
#include <picosha3.h>

#include "types.hh"
#include "eta.hh"
#include "begin.hh"

namespace τ
{


struct isha2
{
  isha2 &operator<<(ηic &i) { return *this << i.all(); }
  isha2 &operator<<(Sn<u8c> const &a)
    { h.process(a.begin(), a.end());
      return *this; }

  h256 operator()()
    { h256 a;
      h.finish();
      h.get_hash_bytes(a.begin(), a.end());
      return a; }

protected:
  picosha2::hash256_one_by_one h;
};


template<uN N>  // N = bytes
struct isha3
{
  isha3 &operator<<(ηic &i) { return *this << i.all(); }
  isha3 &operator<<(Sn<u8c> const &a)
    { h.process(a.begin(), a.end());
      return *this; }

  Ar<u8, N> operator()()
    { Ar<u8, N> a;
      h.finish();
      h.get_hash_bytes(a.begin(), a.end());
      return a; }

protected:
  decltype(picosha3::get_sha3_generator<N>()) h;
};


}

#include "end.hh"

#endif
