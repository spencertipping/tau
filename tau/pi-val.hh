#ifndef τπval_h
#define τπval_h


#include "types.hh"
#include "eta.hh"

#include "begin.hh"

namespace τ
{


struct πv final
{
  Va<
    // C++ native types
    Re,
    V<Sp<πv>>,
    M<St, Sp<πv>>,
    V<i8>,
    V<i16>,
    V<i32>,
    V<i64>,
    V<f32>,
    V<f64>,

    // η-mappable values
    ηsig,
    i64,
    f64,
    St,
    ηatom,
    ηi,
    Sn<i8b>,
    Sn<i16b>,
    Sn<i32b>,
    Sn<i64b>,
    Sn<f32b>,
    Sn<f64b>
    > v;

  template<class T> πv &operator=(T &&x) { v = fo<T>(x); return *this; }
};


typedef πv const πvc;


}

#include "end.hh"


#endif
