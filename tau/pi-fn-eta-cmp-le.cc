#include "pi-fn-eta-cmp.hh"
#include "begin.hh"

namespace τ
{


πdf πηle()  { return [](πi&, πv &&a, πv &&b) { return πv{a <= b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηvle() { return πdv(fn {τbinfallthrough(<=), τvbinop(op_le)}); }


}

#include "end.hh"
