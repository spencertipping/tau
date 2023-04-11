#include "pi-fn-eta-prefix.hh"
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
auto τclang_workaround_gensyms_int()
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


πmf πηinv()    { return πmv(fn {τunfallthrough(~),  τiunop(op_inv)}); }
πmf πηnot()    { return πmv(fn {τunfallthrough(!),  τiunop(op_not)}); }
πmf πηnotnot() { return πmv(fn {τunfallthrough(!!), τiunop(op_notnot)}); }

πdf πηmod() { return πdv(fn {τbinfallthrough(%),  τibinop(op_mod)}); }
πdf πηlsh() { return πdv(fn {τbinfallthrough(<<), τibinop(op_lsh)}); }
πdf πηrsh() { return πdv(fn {τbinfallthrough(>>), τibinop(op_rsh)}); }
πdf πηand() { return πdv(fn {τbinfallthrough(&),  τibinop(op_and)}); }
πdf πηor () { return πdv(fn {τbinfallthrough(|),  τibinop(op_or)}); }
πdf πηxor() { return πdv(fn {τbinfallthrough(^),  τibinop(op_xor)}); }


}

#include "end.hh"
