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
deft(u8,  3, n_int,   i.ci())  // NOTE: upsize to preserve positivity
deft(u16, 5, n_int,   i.ci())
deft(u32, 9, n_int,   i.ci())
deft(u64, 9, n_int,   i.ci())  // NOTE: can overflow into signed
deft(f32, 5, n_float, i.cf())
deft(f64, 9, n_float, i.cf())

deft(bool,  2, atom, i.cb())
deft(ηsig,  2, sig,  i.sig())
deft(ηatom, 2, atom, i.a())

deft(ηname, 16, name,   i.n())
deft(ηbin,  16, binary, ηbin{St{i.bin()}})
deft(Stv,   16, string, i.s())
deft(St,    16, string, St{i.cs()})

deft(Sn<i8bc>,  64,  int8s,    i.i8s())
deft(Sn<i16bc>, 128, int16s,   i.i16s())
deft(Sn<i32bc>, 256, int32s,   i.i32s())
deft(Sn<i64bc>, 512, int64s,   i.i64s())
deft(Sn<f32bc>, 256, float32s, i.f32s())
deft(Sn<f64bc>, 512, float64s, i.f64s())

#undef deft


Tt struct ηauto_<ηY<T>>
{
  sletc ops = ηauto_op_encode | ηauto_op_decode | ηauto_op_type;
  sletc t = ηtype::η;
  sletc n = 64;
  static ηY<T> v(ηic &i) { return {ηauto_<T>::v(i.η())}; }
};

template<class T, ηauto_encode X> ηo<T> &operator<<(ηo<T> &x, ηY<X> const &y)
{
  return x << (ηm{ηauto_<X>::n} << y.x);
}


Tn struct ηauto_<char>
{
  sletc ops = ηauto_op_encode | ηauto_op_decode | ηauto_op_type;
  sletc t   = ηtype::string;
  sletc n   = 1;
  static char v(ηic &i)
    { A(i.is_s(), "ηi → char: not a string");
      A(i.size() == 1, "ηi → char: invalid size " << i.size());
      return i.data()[0]; }
};


template<uN N> struct ηauto_<Ar<u8, N>>
{
  sletc ops = ηauto_op_encode | ηauto_op_decode | ηauto_op_type;
  sletc t   = ηtype::binary;
  sletc n   = N;
  static auto v(ηic &i)
    { A(i.size() == N, "ηi → u8[" << N << "]: invalid size " << i.size());
      A(i.is_bin() || i.is_s(), "ηi → u8[" << N << "]: not a binary or string");
      Ar<u8, N> r;
      for (uN j = 0; j < N; ++j) r[j] = i.data()[j];
      return r; }
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

Tx struct ηauto_<V<X>>
{
  sletc ops = (ηauto_encode<X> ? ηauto_op_encode : 0)
            | (ηauto_decode<X> ? ηauto_op_decode : 0)
            | ηauto_op_type;
  sletc t = ηtype::η;
  sletc n = ηauto_<X>::n * 64;
  static V<X> v(ηi i)
    { V<X> r;
      while (i) r.push_back(ηauto_<X>::v(i)), ++i;
      return r; }
};

template<class X, class Y> struct ηauto_<M<X, Y>>
{
  sletc ops = (ηauto_encode<X> && ηauto_encode<Y> ? ηauto_op_encode : 0)
            | (ηauto_decode<X> && ηauto_decode<Y> ? ηauto_op_decode : 0)
    | ηauto_op_type;
  sletc t = ηtype::η;
  sletc n = (ηauto_<X>::n + ηauto_<Y>::n) * 16;
  static M<X, Y> v(ηi i)
    { M<X, Y> r;
      while (i && i.has_next())
      { let k = i; ++i;
        let v = i; ++i;
        r[ηauto_<X>::v(k)] = ηauto_<Y>::v(v); }
      return r; }
};


Tn struct ηauto_<V<ηi>>
{
  sletc ops = ηauto_op_decode;

  // NOTE: no t or n because V<ηi> is read-only (i.e. we don't serialize
  // V<ηi> back to η directly)
  static V<ηi> v(ηic &i)
    { ηi j = i;
      V<ηi> r;
      r.push_back(j);
      while (j.has_next()) r.push_back(j = j.next());
      return r; }
};


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


namespace std
{

Tt struct tuple_size<τ::ηY<T>> : integral_constant<size_t, tuple_size<T>::value> {};

template<size_t I, class T>
struct tuple_element<I, τ::ηY<T>> { using type = tuple_element_t<I, T>; };

template<size_t I, class T>
auto &get(τ::ηY<T> &x) { return get<I>(x.x); }

template<size_t I, class T>
auto const &get(τ::ηY<T> const &x) { return get<I>(x.x); }

}

#include "end.hh"

#endif
