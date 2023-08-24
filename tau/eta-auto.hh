#ifndef τηauto_h
#define τηauto_h

#include "eta-i.hh"
#include "eta-m.hh"
#include "begin.hh"

namespace τ
{


enum ηauto_ops
{
  ηauto_op_decode = 1,
  ηauto_op_encode = 2,
  ηauto_op_type   = 4,
};


// ηauto_<T> provides information about how T maps to η values, in particular
// the η type, an approximate size in bytes (used for heap reservations), and
// a function to convert from η to T.
Tt struct ηauto_ { sletc ops = 0; };

Tt concept ηauto_encode = (ηauto_<T>::ops & ηauto_op_encode) != 0;
Tt concept ηauto_decode = (ηauto_<T>::ops & ηauto_op_decode) != 0;
Tt concept ηauto_type   = (ηauto_<T>::ops & ηauto_op_type)   != 0;


// ηY<T> contains a T that will be unpacked from an inner η.
Tt struct ηY { T x; };
Tt ηY(T) -> ηY<T>;


#define deft(ct, s, yt, ve)                                             \
  Tn struct ηauto_<ct>                                                  \
  { sletc ops = ηauto_op_decode | ηauto_op_encode | ηauto_op_type;      \
    sletc t = ηtype::yt;                                                \
    sletc n = s;                                                        \
    static ct v(ηic &i) { return ve; } };

deft(ηi, 64, η, i)  // ηis are passed through
deft(ηm, 64, η, i)  // ηms are passed through

deft(i8,  2, n_int,   i.ci())
deft(i16, 3, n_int,   i.ci())
deft(i32, 5, n_int,   i.ci())
deft(i64, 9, n_int,   i.ci())
deft(f32, 5, n_float, i.cf())
deft(f64, 9, n_float, i.cf())

deft(bool,  2, atom, i.cb())
deft(ηsig,  2, sig,  i.sig())
deft(ηatom, 2, atom, i.a())

deft(ηname, 16, name,   ηname{St{i.n()}})
deft(Stv,   16, string, i.s())
deft(St,    16, string, St{i.cs()})

deft(Sn<i8bc>,  64,  int8s,    i.i8s())
deft(Sn<i16bc>, 128, int16s,   i.i16s())
deft(Sn<i32bc>, 256, int32s,   i.i32s())
deft(Sn<i64bc>, 512, int64s,   i.i64s())
deft(Sn<f32bc>, 256, float32s, i.f32s())
deft(Sn<f64bc>, 512, float64s, i.f64s())

Tt struct ηauto_<ηY<T>>
{
  sletc ops = ηauto_op_decode | ηauto_op_type;
  sletc t = ηtype::η;
  sletc n = 64;
  static ηY<T> v(ηic &i) { return {ηauto_<T>::v(i.η())}; }
};


// Inline value, which can be encoded and decoded but does not have
// a type.
Tn struct ηauto_<Sn<u8c>>
{
  sletc ops = ηauto_op_decode | ηauto_op_encode;
  sletc n   = 64;
  static Sn<u8c> v(ηic &i) { return i.all(); }
};


// NOTE: these values require explicit delete[], which is error-prone;
// instead of introducing those problems, we just don't support decoding
// for these.
Tn struct ηauto_<chc*>
{
  sletc ops = ηauto_op_encode | ηauto_op_type;
  sletc t = ηtype::string;
  sletc n = 16;
};

template<uN N>
struct ηauto_<char[N]>
{
  sletc ops = ηauto_op_encode | ηauto_op_type;
  sletc t = ηtype::string;
  sletc n = N;
};


Tx struct ηauto_<T<X>>
{
  sletc ops = ηauto_op_decode | ηauto_op_encode | ηauto_op_type;
  sletc t = ηtype::η;
  sletc n = ηauto_<X>::n;
  static T<X> v(ηic &i) { return {ηauto_<X>::v(i)}; }
};

template<class X, class Y, class... Xs>
struct ηauto_<T<X, Y, Xs...>>
{
  sletc ops = ηauto_op_decode | ηauto_op_encode | ηauto_op_type;
  sletc t = ηtype::η;
  sletc n = ηauto_<X>::n + ηauto_<T<Y, Xs...>>::n;
  static T<X, Y, Xs...> v(ηic &i)
    { return std::tuple_cat(T<X>{ηauto_<X>::v(i)},
                            ηauto_<T<Y, Xs...>>::v(i.next())); }
};

Tn struct ηauto_<V<ηi>>
{
  sletc ops = ηauto_op_encode;

  // NOTE: no t or n because V<ηi> is read-only (i.e. we don't serialize
  // V<ηi> back to η directly)
  static V<ηi> v(ηic &i)
    { ηi j = i;
      V<ηi> r;
      r.push_back(j);
      while (j.has_next()) r.push_back(j = j.next());
      return r; }
};

#undef deft


template<class R, class... Xs>
auto ηhauto_(F<R(Xs...)> &&f)
{
  return [f=mo(f)](ηic &i) -> R
    { return std::apply(f, ηauto_<T<De<Xs>...>>::v(i)); };
}

// Convert a C++ function to one that accepts a ηi to unpack its arguments.
template<class F>
auto ηhauto(F const &f)
{
  return ηhauto_(std::function(f));
}


template<class R, class... Xs, class... Ys>
auto ηvauto_(F<R(Xs...)> &&f)
{
  // NOTE: each Ys is a ηic
  return [f=mo(f)](Ys const&... ys) -> R
    { return f(ηauto_<De<Xs>>::v(ys)...); };
}

// Convert a C++ function to one that accepts a single ηi for each of its
// arguments.
template<class F>
auto ηvauto(F const &f)
{
  return ηvauto_(std::function(f));
}


// Extract a ηi into a structured tuple
Txs T<Xs...> ηT(ηic &i)
{
  return ηauto_<T<Xs...>>::v(i);
}


}

#include "end.hh"

#endif
