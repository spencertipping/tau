#ifndef τπηcmp_h
#define τπηcmp_h

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


}

#include "end.hh"

#endif
