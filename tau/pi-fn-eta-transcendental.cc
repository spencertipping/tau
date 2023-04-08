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
auto τclang_workaround_gensyms_transcendental()
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


#define op_logn(n, base) (std::log(n) / std::log(base))

πdf πηpow()      { return πdv(fn {τbinfallthrough(**),       τnbinop(std::pow)}); }
πdf πηlogn()     { return πdv(fn {τbinfallthrough(logn),     τnbinop(op_logn)}); }
πdf πηcopysign() { return πdv(fn {τbinfallthrough(copysign), τnbinop(std::copysign)}); }
πdf πηhypot()    { return πdv(fn {τbinfallthrough(hypot),    τnbinop(std::hypot)}); }

πmf πηexp()   { return πmv(fn {τunfallthrough(exp),   τnunop(std::exp)}); }
πmf πηexp2()  { return πmv(fn {τunfallthrough(exp2),  τnunop(std::exp2)}); }
πmf πηexpm1() { return πmv(fn {τunfallthrough(expm1), τnunop(std::expm1)}); }
πmf πηlog()   { return πmv(fn {τunfallthrough(log),   τnunop(std::log)}); }
πmf πηlog2()  { return πmv(fn {τunfallthrough(log2),  τnunop(std::log2)}); }
πmf πηlog1p() { return πmv(fn {τunfallthrough(log1p), τnunop(std::log1p)}); }

πmf πηsqrt()  { return πmv(fn {τunfallthrough(sqrt), τnunop(std::sqrt)}); }
πmf πηcbrt()  { return πmv(fn {τunfallthrough(cbrt), τnunop(std::cbrt)}); }

πmf πηsin()   { return πmv(fn {τunfallthrough(sin),    τnunop(std::sin)}); }
πmf πηcos()   { return πmv(fn {τunfallthrough(cos),    τnunop(std::cos)}); }
πmf πηtan()   { return πmv(fn {τunfallthrough(tan),    τnunop(std::tan)}); }
πmf πηasin()  { return πmv(fn {τunfallthrough(asin),   τnunop(std::asin)}); }
πmf πηacos()  { return πmv(fn {τunfallthrough(acos),   τnunop(std::acos)}); }
πmf πηatan()  { return πmv(fn {τunfallthrough(atan),   τnunop(std::atan)}); }
πdf πηatan2() { return πdv(fn {τbinfallthrough(atan2), τnbinop(std::atan2)}); }

πmf πηsinh()  { return πmv(fn {τunfallthrough(sinh),  τnunop(std::sinh)}); }
πmf πηcosh()  { return πmv(fn {τunfallthrough(cosh),  τnunop(std::cosh)}); }
πmf πηtanh()  { return πmv(fn {τunfallthrough(tanh),  τnunop(std::tanh)}); }
πmf πηasinh() { return πmv(fn {τunfallthrough(asinh), τnunop(std::asinh)}); }
πmf πηacosh() { return πmv(fn {τunfallthrough(acosh), τnunop(std::acosh)}); }
πmf πηatanh() { return πmv(fn {τunfallthrough(atanh), τnunop(std::atanh)}); }

πmf πηerf()    { return πmv(fn {τunfallthrough(erf),    τnunop(std::erf)}); }
πmf πηerfc()   { return πmv(fn {τunfallthrough(erfc),   τnunop(std::erfc)}); }
πmf πηtgamma() { return πmv(fn {τunfallthrough(tgamma), τnunop(std::tgamma)}); }
πmf πηlgamma() { return πmv(fn {τunfallthrough(lgamma), τnunop(std::lgamma)}); }

πmf πηceil()   { return πmv(fn {τunfallthrough(ceil),   τnunop(std::ceil)}); }
πmf πηfloor()  { return πmv(fn {τunfallthrough(floor),  τnunop(std::floor)}); }
πmf πηtrunc()  { return πmv(fn {τunfallthrough(trunc),  τnunop(std::trunc)}); }
πmf πηround()  { return πmv(fn {τunfallthrough(round),  τnunop(std::llround)}); }


}

#include "end.hh"
