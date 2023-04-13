#include "pi-fn-eta-prefix.hh"
#include "begin.hh"

namespace τ
{


τclang_workaround_gensyms(transcendental)


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
