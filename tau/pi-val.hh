#ifndef τπval_h
#define τπval_h


#include "types.hh"
#include "eta.hh"

#include "begin.hh"

namespace τ
{


struct πv_;

typedef Va<
  // C++ native types (heap-allocated, mutable)
  Sp<Re>,
  Sp<V<πv_>>,
  Sp<M<St, πv_>>,
  Sp<V<i8>>,
  Sp<V<i16>>,
  Sp<V<i32>>,
  Sp<V<i64>>,
  Sp<V<f32>>,
  Sp<V<f64>>,

  // η-mappable values (immediate, read-only)
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
  > πv;

typedef πv const πvc;


ηtype πvt(πvc&);

PO operator<=>(πvc&, πvc&);


struct πv_ final
{
  πv v;
  operator πv const&() const { return v; }
};


}

#include "end.hh"


#endif
