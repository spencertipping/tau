#include "pi-fn-eta-prefix.hh"
#include "arch.hh"
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
auto τclang_workaround_gensyms_numeric()
{
  Sp<V<i8>>  a{new V<i8>};
  Sp<V<i16>> b{new V<i16>};
  Sp<V<i32>> c{new V<i32>};
  Sp<V<i64>> d{new V<i64>};
  Sp<V<f32>> e{new V<f32>};
  Sp<V<f64>> f{new V<f64>};
  return std::make_tuple(a, b, c, d, e, f);
}
#endif


πmf πηneg() { return πmv(fn {τunfallthrough(neg), τnunop(op_neg)}); }

πdf πηadd() { return πdv(fn {τbinfallthrough(+), τnbinop(op_add)}); }
πdf πηsub() { return πdv(fn {τbinfallthrough(-), τnbinop(op_sub)}); }
πdf πηmul() { return πdv(fn {τbinfallthrough(*), τnbinop(op_mul)}); }
πdf πηdiv() { return πdv(fn {τbinfallthrough(/), τnbinop(op_div)}); }


}

#include "end.hh"
