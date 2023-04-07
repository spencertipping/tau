#include "pi-fn-eta-prefix.hh"
#include "begin.hh"

namespace τ
{


πdf πηmod() { return πdv(fn {τbinfallthrough(%),  τibinop(%)}); }
πdf πηlsh() { return πdv(fn {τbinfallthrough(<<), τibinop(<<)}); }
πdf πηrsh() { return πdv(fn {τbinfallthrough(>>), τibinop(>>)}); }
πdf πηand() { return πdv(fn {τbinfallthrough(&),  τibinop(&)}); }
πdf πηor () { return πdv(fn {τbinfallthrough(|),  τibinop(|)}); }
πdf πηxor() { return πdv(fn {τbinfallthrough(^),  τibinop(^)}); }


}

#include "end.hh"
