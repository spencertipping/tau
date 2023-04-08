#include "pi-fn-eta-prefix.hh"
#include "begin.hh"

namespace τ
{


πdf πηmod() { return πdv(fn {τbinfallthrough(%),  τibinop(op_mod)}); }
πdf πηlsh() { return πdv(fn {τbinfallthrough(<<), τibinop(op_lsh)}); }
πdf πηrsh() { return πdv(fn {τbinfallthrough(>>), τibinop(op_rsh)}); }
πdf πηand() { return πdv(fn {τbinfallthrough(&),  τibinop(op_and)}); }
πdf πηor () { return πdv(fn {τbinfallthrough(|),  τibinop(op_or)}); }
πdf πηxor() { return πdv(fn {τbinfallthrough(^),  τibinop(op_xor)}); }


}

#include "end.hh"
