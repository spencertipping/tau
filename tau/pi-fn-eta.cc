#include <memory>
#include <typeinfo>
#include <vector>

#include "types.hh"
#include "eta.hh"
#include "pi-val.hh"
#include "pi-fn-eta.hh"
#include "pi-fn.hh"
#include "begin.hh"

namespace τ
{


// Type widening logic, specified here to prevent C++ from artificially inflating
// arithmetic results (e.g. i8 + i8 = int, that type of thing).

template<int ibits, int fbits> struct ntype;
template<> struct ntype<8,  0> { typedef i8  t; };
template<> struct ntype<16, 0> { typedef i16 t; };
template<> struct ntype<32, 0> { typedef i32 t; };
template<> struct ntype<64, 0> { typedef i64 t; };

template<> struct ntype<0,  32> { typedef f32 t; };
template<> struct ntype<8,  32> { typedef f32 t; };
template<> struct ntype<16, 32> { typedef f32 t; };
template<> struct ntype<32, 32> { typedef f64 t; };
template<> struct ntype<64, 32> { typedef f64 t; };

template<> struct ntype<0,  64> { typedef f64 t; };
template<> struct ntype<8,  64> { typedef f64 t; };
template<> struct ntype<16, 64> { typedef f64 t; };
template<> struct ntype<32, 64> { typedef f64 t; };
template<> struct ntype<64, 64> { typedef f64 t; };


template<class T> struct ibits_of;
template<> struct ibits_of<bool> { sletc v = 8;  };
template<> struct ibits_of<i8>   { sletc v = 8;  };
template<> struct ibits_of<i16>  { sletc v = 16; };
template<> struct ibits_of<i32>  { sletc v = 32; };
template<> struct ibits_of<i64>  { sletc v = 64; };
template<> struct ibits_of<f32>  { sletc v = 0;  };
template<> struct ibits_of<f64>  { sletc v = 0;  };

template<> struct ibits_of<i8b>  { sletc v = 8;  };
template<> struct ibits_of<i16b> { sletc v = 16; };
template<> struct ibits_of<i32b> { sletc v = 32; };
template<> struct ibits_of<i64b> { sletc v = 64; };
template<> struct ibits_of<f32b> { sletc v = 0;  };
template<> struct ibits_of<f64b> { sletc v = 0;  };


template<class T> struct fbits_of;
template<> struct fbits_of<bool> { sletc v = 0;  };
template<> struct fbits_of<i8>   { sletc v = 0;  };
template<> struct fbits_of<i16>  { sletc v = 0;  };
template<> struct fbits_of<i32>  { sletc v = 0;  };
template<> struct fbits_of<i64>  { sletc v = 0;  };
template<> struct fbits_of<f32>  { sletc v = 32; };
template<> struct fbits_of<f64>  { sletc v = 64; };

template<> struct fbits_of<i8b>  { sletc v = 0;  };
template<> struct fbits_of<i16b> { sletc v = 0;  };
template<> struct fbits_of<i32b> { sletc v = 0;  };
template<> struct fbits_of<i64b> { sletc v = 0;  };
template<> struct fbits_of<f32b> { sletc v = 32; };
template<> struct fbits_of<f64b> { sletc v = 64; };


template<class T, class U, class R = typename ntype<
                             std::max(ibits_of<T>::v, ibits_of<U>::v),
                             std::max(fbits_of<T>::v, fbits_of<U>::v)>::t>
struct binr
{
  typedef R t;
};


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
