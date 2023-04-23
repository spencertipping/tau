#ifndef τηauto_h
#define τηauto_h

#include "eta-i.hh"

#include "begin.hh"

namespace τ
{


template<class T> struct ηauto_;

template<> struct ηauto_<i8>  { static i8  v(ηic &i) { return i.i(); } };
template<> struct ηauto_<i16> { static i16 v(ηic &i) { return i.i(); } };
template<> struct ηauto_<i32> { static i32 v(ηic &i) { return i.i(); } };
template<> struct ηauto_<i64> { static i64 v(ηic &i) { return i.i(); } };
template<> struct ηauto_<f32> { static f32 v(ηic &i) { return i.f(); } };
template<> struct ηauto_<f64> { static f64 v(ηic &i) { return i.f(); } };

template<> struct ηauto_<bool>  { static bool  v(ηic &i) { return i.b(); } };
template<> struct ηauto_<ηsig>  { static ηsig  v(ηic &i) { return i.sig(); } };
template<> struct ηauto_<ηatom> { static ηatom v(ηic &i) { return i.a(); } };

template<> struct ηauto_<ηi>  { static ηi  v(ηic &i) { return i.η(); } };

template<> struct ηauto_<Stv> { static Stv v(ηic &i) { return i.s(); } };
template<> struct ηauto_<St>  { static St  v(ηic &i) { return St{i.s()}; } };

template<> struct ηauto_<Sn<i8bc>>  { static Sn<i8bc>  v(ηic &i) { return i.i8s(); } };
template<> struct ηauto_<Sn<i16bc>> { static Sn<i16bc> v(ηic &i) { return i.i16s(); } };
template<> struct ηauto_<Sn<i32bc>> { static Sn<i32bc> v(ηic &i) { return i.i32s(); } };
template<> struct ηauto_<Sn<i64bc>> { static Sn<i64bc> v(ηic &i) { return i.i64s(); } };
template<> struct ηauto_<Sn<f32bc>> { static Sn<f32bc> v(ηic &i) { return i.f32s(); } };
template<> struct ηauto_<Sn<f64bc>> { static Sn<f64bc> v(ηic &i) { return i.f64s(); } };


// TODO: add φauto-style evaluation for η against functions
// Bidirectional function conversion: ηauto(f)(i) -> T<...>


}

#include "end.hh"

#endif
