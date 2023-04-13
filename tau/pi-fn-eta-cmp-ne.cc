#include "pi-fn-eta-cmp.hh"
#include "begin.hh"

namespace τ
{


πdf πηne()  { return [](πi&, πv &&a, πv &&b) { return πv{a != b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηvne() { return πdv(fn {τbinfallthrough(!=), τvbinop(op_ne)}); }


}

#include "end.hh"
