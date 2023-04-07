#include "pi-fn-eta.cc"
#include "begin.hh"

namespace τ
{


πdf πηlt() { return [](πi&, πv &&a, πv &&b) { return πv{a <  b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηle() { return [](πi&, πv &&a, πv &&b) { return πv{a <= b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηgt() { return [](πi&, πv &&a, πv &&b) { return πv{a >  b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηge() { return [](πi&, πv &&a, πv &&b) { return πv{a >= b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηeq() { return [](πi&, πv &&a, πv &&b) { return πv{a == b ? ηatom::ηtrue : ηatom::ηfalse}; }; }
πdf πηne() { return [](πi&, πv &&a, πv &&b) { return πv{a != b ? ηatom::ηtrue : ηatom::ηfalse}; }; }


πdf πηvlt()  { return πdv(fn {τbinfallthrough(<),  τvbinop(<)}); }
πdf πηvle()  { return πdv(fn {τbinfallthrough(<=), τvbinop(<=)}); }
πdf πηvgt()  { return πdv(fn {τbinfallthrough(>),  τvbinop(>)}); }
πdf πηvge()  { return πdv(fn {τbinfallthrough(>=), τvbinop(>=)}); }
πdf πηveq()  { return πdv(fn {τbinfallthrough(==), τvbinop(==)}); }
πdf πηvne()  { return πdv(fn {τbinfallthrough(!=), τvbinop(!=)}); }


}

#include "end.hh"
