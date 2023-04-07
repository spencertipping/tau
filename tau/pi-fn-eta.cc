#include <typeinfo>

#include "types.hh"
#include "eta.hh"
#include "pi-val.hh"
#include "pi-fn-eta.hh"
#include "pi-fn.hh"
#include "begin.hh"

namespace τ
{


// Type widening logic, specified here because emscripten will default
// to widening everything to int, long long, and double (and consequently
// causing linker errors as we assign incompatible shared_ptrs into πvs).
//
// It's also worth pinning it down in general, since C compilers might
// artificially widen vectorized things.
//
// There's a huge lookup table below because I'm bad at C++. Surely there's
// a better strategy, but I ran into SFINAE problems trying to substitute
// i8b for i8 with template programming.
//
// TODO: replace this garbage with float_bits and int_bits logic to dispatch

template<class T, class U> struct binr;

template<> struct binr<i8, i8>   { typedef i8 t; };
template<> struct binr<i8, i16>  { typedef i16 t; };
template<> struct binr<i8, i32>  { typedef i32 t; };
template<> struct binr<i8, i64>  { typedef i64 t; };
template<> struct binr<i8, f32>  { typedef f32 t; };
template<> struct binr<i8, f64>  { typedef f64 t; };
template<> struct binr<i8, i8b>  { typedef i8 t; };
template<> struct binr<i8, i16b> { typedef i16 t; };
template<> struct binr<i8, i32b> { typedef i32 t; };
template<> struct binr<i8, i64b> { typedef i64 t; };
template<> struct binr<i8, f32b> { typedef f32 t; };
template<> struct binr<i8, f64b> { typedef f64 t; };

template<> struct binr<i16, i8>   { typedef i16 t; };
template<> struct binr<i16, i16>  { typedef i16 t; };
template<> struct binr<i16, i32>  { typedef i32 t; };
template<> struct binr<i16, i64>  { typedef i64 t; };
template<> struct binr<i16, f32>  { typedef f32 t; };
template<> struct binr<i16, f64>  { typedef f64 t; };
template<> struct binr<i16, i8b>  { typedef i16 t; };
template<> struct binr<i16, i16b> { typedef i16 t; };
template<> struct binr<i16, i32b> { typedef i32 t; };
template<> struct binr<i16, i64b> { typedef i64 t; };
template<> struct binr<i16, f32b> { typedef f32 t; };
template<> struct binr<i16, f64b> { typedef f64 t; };

template<> struct binr<i32, i8>   { typedef i32 t; };
template<> struct binr<i32, i16>  { typedef i32 t; };
template<> struct binr<i32, i32>  { typedef i32 t; };
template<> struct binr<i32, i64>  { typedef i64 t; };
template<> struct binr<i32, f32>  { typedef f64 t; };
template<> struct binr<i32, f64>  { typedef f64 t; };
template<> struct binr<i32, i8b>  { typedef i32 t; };
template<> struct binr<i32, i16b> { typedef i32 t; };
template<> struct binr<i32, i32b> { typedef i32 t; };
template<> struct binr<i32, i64b> { typedef i64 t; };
template<> struct binr<i32, f32b> { typedef f64 t; };
template<> struct binr<i32, f64b> { typedef f64 t; };

template<> struct binr<i64, i8>   { typedef i64 t; };
template<> struct binr<i64, i16>  { typedef i64 t; };
template<> struct binr<i64, i32>  { typedef i64 t; };
template<> struct binr<i64, i64>  { typedef i64 t; };
template<> struct binr<i64, f32>  { typedef f64 t; };
template<> struct binr<i64, f64>  { typedef f64 t; };
template<> struct binr<i64, i8b>  { typedef i64 t; };
template<> struct binr<i64, i16b> { typedef i64 t; };
template<> struct binr<i64, i32b> { typedef i64 t; };
template<> struct binr<i64, i64b> { typedef i64 t; };
template<> struct binr<i64, f32b> { typedef f64 t; };
template<> struct binr<i64, f64b> { typedef f64 t; };

template<> struct binr<f32, i8>   { typedef f32 t; };
template<> struct binr<f32, i16>  { typedef f32 t; };
template<> struct binr<f32, i32>  { typedef f64 t; };
template<> struct binr<f32, i64>  { typedef f64 t; };
template<> struct binr<f32, f32>  { typedef f32 t; };
template<> struct binr<f32, f64>  { typedef f64 t; };
template<> struct binr<f32, i8b>  { typedef f32 t; };
template<> struct binr<f32, i16b> { typedef f32 t; };
template<> struct binr<f32, i32b> { typedef f64 t; };
template<> struct binr<f32, i64b> { typedef f64 t; };
template<> struct binr<f32, f32b> { typedef f32 t; };
template<> struct binr<f32, f64b> { typedef f64 t; };

template<> struct binr<f64, i8>   { typedef f64 t; };
template<> struct binr<f64, i16>  { typedef f64 t; };
template<> struct binr<f64, i32>  { typedef f64 t; };
template<> struct binr<f64, i64>  { typedef f64 t; };
template<> struct binr<f64, f32>  { typedef f64 t; };
template<> struct binr<f64, f64>  { typedef f64 t; };
template<> struct binr<f64, i8b>  { typedef f64 t; };
template<> struct binr<f64, i16b> { typedef f64 t; };
template<> struct binr<f64, i32b> { typedef f64 t; };
template<> struct binr<f64, i64b> { typedef f64 t; };
template<> struct binr<f64, f32b> { typedef f64 t; };
template<> struct binr<f64, f64b> { typedef f64 t; };

template<> struct binr<i8b, i8>   { typedef i8 t; };
template<> struct binr<i8b, i16>  { typedef i16 t; };
template<> struct binr<i8b, i32>  { typedef i32 t; };
template<> struct binr<i8b, i64>  { typedef i64 t; };
template<> struct binr<i8b, f32>  { typedef f32 t; };
template<> struct binr<i8b, f64>  { typedef f64 t; };
template<> struct binr<i8b, i8b>  { typedef i8 t; };
template<> struct binr<i8b, i16b> { typedef i16 t; };
template<> struct binr<i8b, i32b> { typedef i32 t; };
template<> struct binr<i8b, i64b> { typedef i64 t; };
template<> struct binr<i8b, f32b> { typedef f32 t; };
template<> struct binr<i8b, f64b> { typedef f64 t; };

template<> struct binr<i16b, i8>   { typedef i16 t; };
template<> struct binr<i16b, i16>  { typedef i16 t; };
template<> struct binr<i16b, i32>  { typedef i32 t; };
template<> struct binr<i16b, i64>  { typedef i64 t; };
template<> struct binr<i16b, f32>  { typedef f32 t; };
template<> struct binr<i16b, f64>  { typedef f64 t; };
template<> struct binr<i16b, i8b>  { typedef i16 t; };
template<> struct binr<i16b, i16b> { typedef i16 t; };
template<> struct binr<i16b, i32b> { typedef i32 t; };
template<> struct binr<i16b, i64b> { typedef i64 t; };
template<> struct binr<i16b, f32b> { typedef f32 t; };
template<> struct binr<i16b, f64b> { typedef f64 t; };

template<> struct binr<i32b, i8>   { typedef i32 t; };
template<> struct binr<i32b, i16>  { typedef i32 t; };
template<> struct binr<i32b, i32>  { typedef i32 t; };
template<> struct binr<i32b, i64>  { typedef i64 t; };
template<> struct binr<i32b, f32>  { typedef f64 t; };
template<> struct binr<i32b, f64>  { typedef f64 t; };
template<> struct binr<i32b, i8b>  { typedef i32 t; };
template<> struct binr<i32b, i16b> { typedef i32 t; };
template<> struct binr<i32b, i32b> { typedef i32 t; };
template<> struct binr<i32b, i64b> { typedef i64 t; };
template<> struct binr<i32b, f32b> { typedef f64 t; };
template<> struct binr<i32b, f64b> { typedef f64 t; };

template<> struct binr<i64b, i8>   { typedef i64 t; };
template<> struct binr<i64b, i16>  { typedef i64 t; };
template<> struct binr<i64b, i32>  { typedef i64 t; };
template<> struct binr<i64b, i64>  { typedef i64 t; };
template<> struct binr<i64b, f32>  { typedef f64 t; };
template<> struct binr<i64b, f64>  { typedef f64 t; };
template<> struct binr<i64b, i8b>  { typedef i64 t; };
template<> struct binr<i64b, i16b> { typedef i64 t; };
template<> struct binr<i64b, i32b> { typedef i64 t; };
template<> struct binr<i64b, i64b> { typedef i64 t; };
template<> struct binr<i64b, f32b> { typedef f64 t; };
template<> struct binr<i64b, f64b> { typedef f64 t; };

template<> struct binr<f32b, i8>   { typedef f32 t; };
template<> struct binr<f32b, i16>  { typedef f32 t; };
template<> struct binr<f32b, i32>  { typedef f64 t; };
template<> struct binr<f32b, i64>  { typedef f64 t; };
template<> struct binr<f32b, f32>  { typedef f32 t; };
template<> struct binr<f32b, f64>  { typedef f64 t; };
template<> struct binr<f32b, i8b>  { typedef f32 t; };
template<> struct binr<f32b, i16b> { typedef f32 t; };
template<> struct binr<f32b, i32b> { typedef f64 t; };
template<> struct binr<f32b, i64b> { typedef f64 t; };
template<> struct binr<f32b, f32b> { typedef f32 t; };
template<> struct binr<f32b, f64b> { typedef f64 t; };

template<> struct binr<f64b, i8>   { typedef f64 t; };
template<> struct binr<f64b, i16>  { typedef f64 t; };
template<> struct binr<f64b, i32>  { typedef f64 t; };
template<> struct binr<f64b, i64>  { typedef f64 t; };
template<> struct binr<f64b, f32>  { typedef f64 t; };
template<> struct binr<f64b, f64>  { typedef f64 t; };
template<> struct binr<f64b, i8b>  { typedef f64 t; };
template<> struct binr<f64b, i16b> { typedef f64 t; };
template<> struct binr<f64b, i32b> { typedef f64 t; };
template<> struct binr<f64b, i64b> { typedef f64 t; };
template<> struct binr<f64b, f32b> { typedef f64 t; };
template<> struct binr<f64b, f64b> { typedef f64 t; };


#define τbinfallthrough(op)                             \
  [](πi &i, auto &&x, auto &&y) -> πv                   \
  { i.fail((Ss{} << #op << " does not apply to "        \
            << typeid(x).name() << " and "              \
            << typeid(y).name()).str()); }


// Yes, it's true. After hours hunting this one down, it seems that WASM
// has a link-time issue having something to do with missing function
// deleters being invoked from a shared_ptr context (maybe?) and cannot
// build this code. I've tried many things, including upgrading the emsdk
// docker image, to no avail. The only fix is to drop emscripten vectors
// entirely, at least until I build up more courage to try this again.

#if τemscripten
# define τspan_binop(t, op, a, b)                       \
  { i.fail("no vectorized op support for webassembly"); }
#else
# define τspan_binop(t, op, a, b)                                       \
  { let     &x = (a);                                                   \
    let     &y = (b);                                                   \
    let      s = std::min(x.size(), y.size());                          \
    Sp<V<t>> r{new V<t>};                                               \
    r->reserve(s);                                                      \
    for (uN i = 0; i < s; ++i) (*r)[i] = x[i] op y[i];                  \
    return πv{r}; }
#endif


#define τbinoptype(op, a, b) decltype(std::declval<a>() op std::declval<b>())
#define τbinrtype( op, a, b) typename binr<T, U>::t

#define τvbinop(op)                                                     \
  []<class T, class U, class = τbinrtype(op, T, U)>                     \
  (πi &i, T a, Sn<U> b) -> πv                                           \
  { τspan_binop(τbinrtype(op, T, U), op, πbcast{a}, b); },              \
                                                                        \
  []<class T, class U, class = τbinrtype(op, T, U)>                     \
  (πi &i, Sn<T> a, U b) -> πv                                           \
  { τspan_binop(τbinrtype(op, T, U), op, a, πbcast{b}); },              \
                                                                        \
  []<class T, class U, class = τbinrtype(op, T, U)>                     \
  (πi &i, T a, Sp<V<U>> b) -> πv                                        \
  { τspan_binop(τbinrtype(op, T, U), op, πbcast{a}, *b); },             \
                                                                        \
  []<class T, class U, class = τbinrtype(op, T, U)>                     \
  (πi &i, Sp<V<T>> a, U b) -> πv                                        \
  { τspan_binop(τbinrtype(op, T, U), op, *a, πbcast{b}); },             \
                                                                        \
  []<class T, class U, class = τbinrtype(op, T, U)>                     \
  (πi &i, Sn<T> a, Sn<U> b) -> πv                                       \
  { τspan_binop(τbinrtype(op, T, U), op, a, b); },                      \
                                                                        \
  []<class T, class U, class = τbinrtype(op, T, U)>                     \
  (πi &i, Sp<V<T>> a, Sn<U> b) -> πv                                    \
  { τspan_binop(τbinrtype(op, T, U), op, *a, b); },                     \
                                                                        \
  []<class T, class U, class = τbinrtype(op, T, U)>                     \
  (πi &i, Sn<T> a, Sp<V<U>> b) -> πv                                    \
  { τspan_binop(τbinrtype(op, T, U), op, a, *b); },                     \
                                                                        \
  []<class T, class U, class = τbinrtype(op, T, U)>                     \
  (πi &i, Sp<V<T>> a, Sp<V<U>> b) -> πv                                 \
  { τspan_binop(τbinrtype(op, T, U), op, *a, *b); }


#define τibinop(op)                                     \
  [](πi&, i64 a, i64 b) -> πv { return {a op b}; }

#define τfbinop(op)                                     \
  [](πi&, f64 a, f64 b) -> πv { return {a op b}; }

#define τmbinop(op)                                     \
  [](πi&, i64 a, f64 b) -> πv { return {a op b}; },     \
  [](πi&, f64 a, i64 b) -> πv { return {a op b}; }

#define τnbinop(op)                             \
  τibinop(op),                                  \
  τfbinop(op),                                  \
  τmbinop(op),                                  \
  τvbinop(op)


πdf πηadd() { return πdv(fn {τbinfallthrough(+), τnbinop(+)}); }

/*
πdf πηsub() { return πdv(fn {τbinfallthrough(-), τnbinop(-)}); }
πdf πηmul() { return πdv(fn {τbinfallthrough(*), τnbinop(*)}); }
πdf πηdiv() { return πdv(fn {τbinfallthrough(/), τnbinop(/)}); }
πdf πηmod() { return πdv(fn {τbinfallthrough(%), τibinop(%)}); }

πdf πηlsh() { return πdv(fn {τbinfallthrough(<<), τibinop(<<)}); }
πdf πηrsh() { return πdv(fn {τbinfallthrough(>>), τibinop(>>)}); }
πdf πηand() { return πdv(fn {τbinfallthrough(&),  τibinop(&)}); }
πdf πηor () { return πdv(fn {τbinfallthrough(|),  τibinop(|)}); }
πdf πηxor() { return πdv(fn {τbinfallthrough(^),  τibinop(^)}); }
*/


// TODO: define the right behavior for these (i.e. do we use η ordering,
// or do we broadcast for vector ops?)
/*
πdf πηlt()  { return πdv(fn {τbinfallthrough(<),  τnbinop(<)}); }
πdf πηle()  { return πdv(fn {τbinfallthrough(<=), τnbinop(<=)}); }
πdf πηgt()  { return πdv(fn {τbinfallthrough(>),  τnbinop(>)}); }
πdf πηge()  { return πdv(fn {τbinfallthrough(>=), τnbinop(>=)}); }
πdf πηeq()  { return πdv(fn {τbinfallthrough(==), τnbinop(==)}); }
πdf πηne()  { return πdv(fn {τbinfallthrough(!=), τnbinop(!=)}); }
*/


}

#include "end.hh"
