#include "pi-fn-eta-prefix.hh"
#include "begin.hh"

namespace τ
{


τclang_workaround_gensyms(int)


πmf πηinv()    { return πmv(fn {τunfallthrough(~),  τiunop(op_inv)}); }
πmf πηnot()    { return πmv(fn {τunfallthrough(!),  τiunop(op_not)}); }
πmf πηnotnot() { return πmv(fn {τunfallthrough(!!), τiunop(op_notnot)}); }


}

#include "end.hh"
