#ifndef τηauto_h
#define τηauto_h

#include "eta-i.hh"
#include "begin.hh"

namespace τ
{


// ηauto_<T> provides information about how T maps to η values, in particular
// the η type, an approximate size in bytes (used for heap reservations), and
// a function to convert from η to T.
Tt struct ηauto_;


#define deft(ct, s, yt, ve)                    \
  template<> struct ηauto_<ct>                 \
  { sletc t = ηtype::yt;                       \
    sletc n = s;                               \
    static ct v(ηic &i) { return ve; } };

deft(ηi, 64, η, i)  // ηis are passed through

deft(i8,  2, n_int,   i.i())
deft(i16, 3, n_int,   i.i())
deft(i32, 5, n_int,   i.i())
deft(i64, 9, n_int,   i.i())
deft(f32, 5, n_float, i.f())
deft(f64, 9, n_float, i.f())

deft(bool,  2, atom, i.b())
deft(ηsig,  2, sig,  i.sig())
deft(ηatom, 2, atom, i.a())


deft(Stv,  16, string, i.s())
deft(St,   16, string, St{i.s()})
deft(chc*, 16, string, (A(0, "η → const char*"), nullptr))

deft(Sn<i8bc>,  64,  int8s,    i.i8s())
deft(Sn<i16bc>, 128, int16s,   i.i16s())
deft(Sn<i32bc>, 256, int32s,   i.i32s())
deft(Sn<i64bc>, 512, int64s,   i.i64s())
deft(Sn<f32bc>, 256, float32s, i.f32s())
deft(Sn<f64bc>, 512, float64s, i.f64s())

template<uN N>
struct ηauto_<char[N]>
{
  sletc t = ηtype::string;
  sletc n = N;
  static chc *v(ηic &i) { A(0, "η → char[" << N << "]"); return nullptr; }
};

template<class X>
struct ηauto_<T<X>>
{
  sletc t = ηtype::η;
  sletc n = ηauto_<X>::n;
  static T<X> v(ηic &i) { return {ηauto_<X>::v(i)}; }
};

template<class X, class Y, class... Xs>
struct ηauto_<T<X, Y, Xs...>>
{
  sletc t = ηtype::η;
  sletc n = ηauto_<X>::n + ηauto_<T<Y, Xs...>>::n;
  static T<X, Y, Xs...> v(ηic &i)
    { return std::tuple_cat(T<X>{ηauto_<X>::v(i)},
                            ηauto_<T<Y, Xs...>>::v(i.next())); }
};

#undef deft


// Convert a C++ function to one that accepts a ηi to unpack its arguments.
template<class R, class... Xs>
auto ηhauto(F<R(Xs...)> &&f)
{
  return [f=mo(f)](ηic &i) -> R
    { return std::apply(f, ηauto_<T<Xs...>>::v(i)); };
}

template<class R, class... Xs>
auto ηhauto(R(*f)(Xs...))
{
  return [=](ηic &i) -> R
    { return std::apply(f, ηauto_<T<Xs...>>::v(i)); };
}


// Convert a C++ function to one that accepts a single ηi for each of its
// arguments.
template<class R, class... Xs, class... Ys>
auto ηvauto(F<R(Xs...)> &&f)
{
  // NOTE: each Ys is a ηic
  return [f=mo(f)](Ys const&... ys) -> R
    { return f(ηauto_<Xs>::v(ys)...); };
}

template<class R, class... Xs, class... Ys>
auto ηvauto(R(*f)(Xs...))
{
  return [=](Ys const&... ys) -> R
    { return f(ηauto_<Xs>::v(ys)...); };
}


}

#include "end.hh"

#endif
