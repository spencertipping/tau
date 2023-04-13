#include "pi-fn-eta-cmp.hh"
#include "begin.hh"

namespace τ
{


πdf πηge()  { return [](πi&, πv &&a, πv &&b) { return πv{a >= b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηvge() { return πdv(fn {τbinfallthrough(>=), τvbinop(op_ge)}); }


}

#include "end.hh"
