#ifndef τπint_h
#define τπint_h

#include "xiio.hh"
#include "pi-heap.hh"
#include "begin.hh"

namespace τ
{


// π program interpreter

// TODO: add stack accessors
struct πi final
{
protected:
  πh   h;
  πhsv s{h};
};


}

#include "end.hh"

#endif
