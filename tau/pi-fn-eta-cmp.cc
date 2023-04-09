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
auto τclang_workaround_gensyms_cmp()
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


sc i8 cmp(PO x)
{
  return x == PO::equivalent ?  0
       : x == PO::less       ? -1
       : x == PO::greater    ?  1
       :                        2;
}


#define op_cmp(a, b) cmp(a <=> b)


πdf πηlt()  { return [](πi&, πv &&a, πv &&b) { return πv{a <  b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηle()  { return [](πi&, πv &&a, πv &&b) { return πv{a <= b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηgt()  { return [](πi&, πv &&a, πv &&b) { return πv{a >  b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηge()  { return [](πi&, πv &&a, πv &&b) { return πv{a >= b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηeq()  { return [](πi&, πv &&a, πv &&b) { return πv{a == b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηne()  { return [](πi&, πv &&a, πv &&b) { return πv{a != b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηcmp() { return [](πi&, πv &&a, πv &&b) { return πv{cmp(a <=> b)}; }; }


πdf πηvlt()  { return πdv(fn {τbinfallthrough(<),   τvbinop(op_lt)}); }
πdf πηvle()  { return πdv(fn {τbinfallthrough(<=),  τvbinop(op_le)}); }
πdf πηvgt()  { return πdv(fn {τbinfallthrough(>),   τvbinop(op_gt)}); }
πdf πηvge()  { return πdv(fn {τbinfallthrough(>=),  τvbinop(op_ge)}); }
πdf πηveq()  { return πdv(fn {τbinfallthrough(==),  τvbinop(op_eq)}); }
πdf πηvne()  { return πdv(fn {τbinfallthrough(!=),  τvbinop(op_ne)}); }
πdf πηvcmp() { return πdv(fn {τbinfallthrough(<=>), τvbinop(op_cmp)}); }


}

#include "end.hh"
