#include "pi-fn-eta-cmp.hh"
#include "begin.hh"

namespace τ
{


πdf πηlt()  { return [](πi&, πv &&a, πv &&b) { return πv{a < b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηvlt() { return πdv(fn {τbinfallthrough(<), τvbinop(op_lt)}); }


}

#include "end.hh"
