#include "pi-fn-eta-cmp.hh"
#include "begin.hh"

namespace τ
{


πdf πηeq()  { return [](πi&, πv &&a, πv &&b) { return πv{a == b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηveq() { return πdv(fn {τbinfallthrough(==), τvbinop(op_eq)}); }


}

#include "end.hh"
