#include "pi-fn-eta-prefix.hh"
#include "begin.hh"

namespace τ
{


τclang_workaround_gensyms(numeric)

πmf πηneg() { return πmv(fn {τunfallthrough(neg), τnunop(op_neg)}); }


}

#include "end.hh"
