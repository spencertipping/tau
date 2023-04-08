#include "pi-fn-eta-prefix.hh"
#include "begin.hh"

namespace τ
{


sc i8 cmp(PO x)
{
  return x == PO::equivalent ?  0
       : x == PO::less       ? -1
       : x == PO::greater    ?  1
       :                        2;
}


#define op_cmp(a, b) cmp(a <=> b)


πdf πηlt()  { return [](πi&, πv &&a, πv &&b) { return πv{a <  b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηle()  { return [](πi&, πv &&a, πv &&b) { return πv{a <= b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηgt()  { return [](πi&, πv &&a, πv &&b) { return πv{a >  b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηge()  { return [](πi&, πv &&a, πv &&b) { return πv{a >= b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηeq()  { return [](πi&, πv &&a, πv &&b) { return πv{a == b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηne()  { return [](πi&, πv &&a, πv &&b) { return πv{a != b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηcmp() { return [](πi&, πv &&a, πv &&b) { return πv{cmp(a <=> b)}; }; }


πdf πηvlt()  { return πdv(fn {τbinfallthrough(<),   τvbinop(op_lt)}); }
πdf πηvle()  { return πdv(fn {τbinfallthrough(<=),  τvbinop(op_le)}); }
πdf πηvgt()  { return πdv(fn {τbinfallthrough(>),   τvbinop(op_gt)}); }
πdf πηvge()  { return πdv(fn {τbinfallthrough(>=),  τvbinop(op_ge)}); }
πdf πηveq()  { return πdv(fn {τbinfallthrough(==),  τvbinop(op_eq)}); }
πdf πηvne()  { return πdv(fn {τbinfallthrough(!=),  τvbinop(op_ne)}); }
πdf πηvcmp() { return πdv(fn {τbinfallthrough(<=>), τvbinop(op_cmp)}); }


}

#include "end.hh"
