#include "pi-fn-eta-prefix.hh"
#include "begin.hh"

namespace τ
{


πdf πηatan2() { return πdv(fn {τbinfallthrough(atan2), τnbinop(std::atan2)}); }


}

#include "end.hh"
