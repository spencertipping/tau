#include "pi-fn-eta-cmp.hh"
#include "begin.hh"

namespace τ
{


πdf πηgt()  { return [](πi&, πv &&a, πv &&b) { return πv{a > b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηvgt() { return πdv(fn {τbinfallthrough(>), τvbinop(op_gt)}); }


}

#include "end.hh"
