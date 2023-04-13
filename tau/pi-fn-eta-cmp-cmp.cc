#include "pi-fn-eta-cmp.hh"
#include "begin.hh"

namespace τ
{


πdf πηcmp()  { return [](πi&, πv &&a, πv &&b) { return πv{cmp(a <=> b)}; }; }
πdf πηvcmp() { return πdv(fn {τbinfallthrough(<=>), τvbinop(op_cmp)}); }


}

#include "end.hh"
