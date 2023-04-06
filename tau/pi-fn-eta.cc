#include "pi-fn-eta.hh"
#include "pi-fn.hh"
#include "begin.hh"

namespace τ
{


#define τbinfallthrough(op)                             \
  [](πi &i, auto &&x, auto &&y) -> πv                   \
  { i.fail((Ss{} << #op << " does not apply to "        \
            << typeid(x).name() << " and "              \
            << typeid(y).name()).str()); }


#define τspan_binop(t, op, a, b)                                        \
  { let     &x = (a);                                                   \
    let     &y = (b);                                                   \
    let      s = std::min(x.size(), y.size());                          \
    Sp<V<t>> r{new V<t>}; r->reserve(s);                                \
    for (uN i = 0; i < s; ++i) (*r)[i] = x[i] op y[i];                  \
    return πv{r}; }


#define τbinoptype(op, a, b) decltype(std::declval<a> op std::declval<b>)

#define τvbinop(op)                                                     \
  []<class T, class U, class = τbinoptype(op, T, U)>                    \
  (πi&, T a, Sn<U> b) -> πv                                             \
  { τspan_binop(τbinoptype(op, T, U), op, πbcast{a}, b); },             \
                                                                        \
  []<class T, class U, class = τbinoptype(op, T, U)>                    \
  (πi&, Sn<T> a, U b) -> πv                                             \
  { τspan_binop(τbinoptype(op, T, U), op, a, πbcast{b}); },             \
                                                                        \
  []<class T, class U, class = τbinoptype(op, T, U)>                    \
  (πi&, T a, Sp<V<U>> b) -> πv                                          \
  { τspan_binop(τbinoptype(op, T, U), op, πbcast{a}, *b); },            \
                                                                        \
  []<class T, class U, class = τbinoptype(op, T, U)>                    \
  (πi&, Sp<V<T>> a, U b) -> πv                                          \
  { τspan_binop(τbinoptype(op, T, U), op, *a, πbcast{b}); },            \
                                                                        \
  []<class T, class U, class = τbinoptype(op, T, U)>                    \
  (πi&, Sn<T> a, Sn<U> b) -> πv                                         \
  { τspan_binop(τbinoptype(op, T, U), op, a, b); },                     \
                                                                        \
  []<class T, class U, class = τbinoptype(op, T, U)>                    \
  (πi&, Sp<V<T>> a, Sn<U> b) -> πv                                      \
  { τspan_binop(τbinoptype(op, T, U), op, *a, b); },                    \
                                                                        \
  []<class T, class U, class = τbinoptype(op, T, U)>                    \
  (πi&, Sn<T> a, Sp<V<U>> b) -> πv                                      \
  { τspan_binop(τbinoptype(op, T, U), op, a, *b); },                    \
                                                                        \
  []<class T, class U, class = τbinoptype(op, T, U)>                    \
  (πi&, Sp<V<T>> a, Sp<V<U>> b) -> πv                                   \
  { τspan_binop(τbinoptype(op, T, U), op, *a, *b); }


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
πdf πηsub() { return πdv(fn {τbinfallthrough(-), τnbinop(-)}); }
πdf πηmul() { return πdv(fn {τbinfallthrough(*), τnbinop(*)}); }
πdf πηdiv() { return πdv(fn {τbinfallthrough(/), τnbinop(/)}); }
πdf πηmod() { return πdv(fn {τbinfallthrough(%), τibinop(%)}); }

πdf πηlsh() { return πdv(fn {τbinfallthrough(<<), τibinop(<<)}); }
πdf πηrsh() { return πdv(fn {τbinfallthrough(>>), τibinop(>>)}); }
πdf πηand() { return πdv(fn {τbinfallthrough(&),  τibinop(&)}); }
πdf πηor () { return πdv(fn {τbinfallthrough(|),  τibinop(|)}); }
πdf πηxor() { return πdv(fn {τbinfallthrough(^),  τibinop(^)}); }


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
