// NOTE:
// This file is included outside of the main τ flow. It's a resource
// for split .cc files that are separated to improve build parallelism,
// but otherwise serve as a single module. This allows us to add new
// operators without significantly increasing the total build time,
// assuming we have a lot of processors to work with.

#ifdef τplatform
# error pi-fn-eta-prefix.hh cannot be used as a normal header
#endif


#include <cstdlib>
#include <cmath>
#include <typeinfo>

#include "types.hh"
#include "eta.hh"
#include "pi-val.hh"
#include "pi-fn-eta.hh"
#include "pi-fn.hh"
#include "begin.hh"

namespace τ
{


// WTF, I hear you asking. What purpose could this possibly serve???
// Well, it's a long and tragic tale but to summarize, clang++ has
// a known defect (https://github.com/llvm/llvm-project/issues/44214)
// that causes it to not emit all symbols for templates expanded within
// some types of lambdas. So if we're compiling with clang, we need to
// manually generate these symbols by instantiating them directly.

#if τclang
# define τclang_workaround_gensyms(id)           \
  auto τclang_workaround_gensyms_##id()          \
  {                                              \
    Sp<V<i8>>  a{new V<i8>};                     \
    Sp<V<i16>> b{new V<i16>};                    \
    Sp<V<i32>> c{new V<i32>};                    \
    Sp<V<i64>> d{new V<i64>};                    \
    Sp<V<f32>> e{new V<f32>};                    \
    Sp<V<f64>> f{new V<f64>};                    \
    return std::make_tuple(a, b, c, d, e, f);    \
  }
#else
# define τclang_workaround_gensyms(id)
#endif


// Make a single value act like a vector; this way we can use it with
// vector functions
template<class T>
struct πbcast
{
  T x;
  T     operator[](uN i) const { return x; }
  std::size_t size()     const { return Nl<std::size_t>::max(); }
};

template<class T> πbcast(T) -> πbcast<T>;


// Type widening logic, specified here to prevent C++ from artificially inflating
// arithmetic results (e.g. i8 + i8 = int, that type of thing).
//
// We also vector-encode bools as i8s so we can do vectorized comparisons.

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


template<class T,
         class R = typename ntype<
           ibits_of<De<T>>::v,
           fbits_of<De<T>>::v>::t>
struct unr
{
  typedef R t;
};

template<class T, class U,
         class R = typename ntype<
           std::max(ibits_of<De<T>>::v, ibits_of<De<U>>::v),
           std::max(fbits_of<De<T>>::v, fbits_of<De<U>>::v)>::t>
struct binr
{
  typedef R t;
};


template<class T>
ic auto tnorm(T x) { return Sc<typename unr<T>::t>(x); }


#define τunfallthrough(op)                              \
  [](πi &i, auto &&x) -> πv                             \
  { i.fail((Ss{} << #op << " does not apply to "        \
            << typeid(x).name()).str()); }

#define τbinfallthrough(op)                             \
  [](πi &i, auto &&x, auto &&y) -> πv                   \
  { i.fail((Ss{} << #op << " does not apply to "        \
            << typeid(x).name() << " and "              \
            << typeid(y).name()).str()); }


#define τspan_binop(t, op, a, b)                                        \
  { let     &x = (a);                                                   \
    let     &y = (b);                                                   \
    let      s = std::min(x.size(), y.size());                          \
    Sp<V<t>> r{new V<t>};                                               \
    r->reserve(s);                                                      \
    for (uN i = 0; i < s; ++i) r->push_back(op(tnorm(x[i]), tnorm(y[i]))); \
    return πv{r}; }


#define τspan_unop(t, op, a)                                    \
  { let     &x = (a);                                           \
    let      s = x.size();                                      \
    Sp<V<t>> r{new V<t>};                                       \
    r->reserve(s);                                              \
    for (uN i = 0; i < s; ++i) r->push_back(op(tnorm(x[i])));   \
    return πv{r}; }


#define τunrtype(op, a)     decltype(op(std::declval<a>()))
#define τbinrtype(op, a, b) typename binr<a, b>::t

#define τvunop(op)                              \
  []<class T, class = τunrtype(op, T)>          \
  (πi &i, Sn<T> a) -> πv                        \
  { τspan_unop(τunrtype(op, T), op, a); },      \
                                                \
  []<class T, class = τunrtype(op, T)>          \
  (πi &i, Sp<V<T>> a) -> πv                     \
  { τspan_unop(τunrtype(op, T), op, *a); }

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


#define τiunop(op) [](πi&, i64 a) -> πv { return {op(a)}; }
#define τfunop(op) [](πi&, f64 a) -> πv { return {op(a)}; }
#define τnunop(op) τiunop(op), τfunop(op)


#define τibinop(op)                                     \
  [](πi&, i64 a, i64 b) -> πv { return {op(a, b)}; }

#define τfbinop(op)                                     \
  [](πi&, f64 a, f64 b) -> πv { return {op(a, b)}; }

#define τmbinop(op)                                     \
  [](πi&, i64 a, f64 b) -> πv { return {op(a, b)}; },   \
  [](πi&, f64 a, i64 b) -> πv { return {op(a, b)}; }

#define τnbinop(op)                             \
  τibinop(op),                                  \
  τfbinop(op),                                  \
  τmbinop(op),                                  \
  τvbinop(op)


#define op_add(a, b) a + b
#define op_sub(a, b) a - b
#define op_mul(a, b) a * b
#define op_div(a, b) a / b
#define op_mod(a, b) a % b
#define op_lsh(a, b) a << b
#define op_rsh(a, b) a >> b
#define op_and(a, b) a & b
#define op_or( a, b) a | b
#define op_xor(a, b) a ^ b

#define op_lt(a, b) a < b
#define op_le(a, b) a <= b
#define op_gt(a, b) a > b
#define op_ge(a, b) a >= b
#define op_eq(a, b) a == b
#define op_ne(a, b) a != b


#define op_neg(a)    (-(a))
#define op_inv(a)    (~(a))
#define op_not(a)    (!(a))
#define op_notnot(a) (!!(a))


}

#include "end.hh"
