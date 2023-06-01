#ifndef τηauto_h
#define τηauto_h

#include "eta-i.hh"

#include "begin.hh"

namespace τ
{


// TODO: implement new system as per doc/eta-cpp.md
// TODO: template function that creates ηts from function args
// TODO: function to convert ηi to T<...>, then ηauto = std::apply


Tt struct ηauto_;

#define deft(ct, yt, ve) \
  template<> struct ηauto_<ct> { sletc t = ηtype::yt; static ct v(ηic &i) { return ve; } };

deft(i8,  n_int,   i.i())
deft(i16, n_int,   i.i())
deft(i32, n_int,   i.i())
deft(i64, n_int,   i.i())
deft(f32, n_float, i.f())
deft(f64, n_float, i.f())

deft(bool,  atom, i.b())
deft(ηsig,  sig,  i.sig())
deft(ηatom, atom, i.a())

deft(ηi, η, i)

deft(Stv, string, i.s())
deft(St,  string, St{i.s()})

deft(Sn<i8bc>,  int8s,    i.i8s())
deft(Sn<i16bc>, int16s,   i.i16s())
deft(Sn<i32bc>, int32s,   i.i32s())
deft(Sn<i64bc>, int64s,   i.i64s())
deft(Sn<f32bc>, float32s, i.f32s())
deft(Sn<f64bc>, float64s, i.f64s())

template<class X>
struct ηauto_<T<X>>
{
  sletc t = ηtype::η;
  static T<X> v(ηic &i) { return {ηauto_<X>::v(i)}; }
};

template<class X, class Y, class... Xs>
struct ηauto_<T<X, Y, Xs...>>
{
  sletc t = ηtype::η;
  static T<X, Y, Xs...> v(ηic &i)
    { return std::tuple_cat({ηauto_<X>::v(i)},
                            ηauto_<T<Y, Xs...>>::v(i.next())); }
};

#undef deft


template<uS I, class R, class... Xs, class... Ys>
R ηauto__(F<R(Xs...)> const &f, Sn<u8c> i, Ys&&... ys)
{
  if constexpr (I == sizeof...(Xs)) return f(std::forward<Ys>(ys)...);
  else
  {
    ηi    j = i;
    auto  x = ηauto_<std::tuple_element_t<I, T<Xs...>>>::v(j);
    return ηauto__<I + 1>(f, j.after(), std::forward<Ys>(ys)..., x);
  }
}


template<class R, class... Xs>
auto ηauto(F<R(Xs...)> const &f)
{
  return [=](ηi const &i) -> R { return ηauto__<0>(f, i.all()); };
}

template<class R, class... Xs>
auto ηauto(R(*f)(Xs...))
{
  return ηauto(F<R(Xs...)>(f));
}


}

#include "end.hh"

#endif
