#include "pi-fn-eta-prefix.hh"
#include "begin.hh"

namespace τ
{


#define op_logn(n, base) (std::log(n) / std::log(base))
πdf πηlogn() { return πdv(fn {τbinfallthrough(logn), τnbinop(op_logn)}); }


}

#include "end.hh"
