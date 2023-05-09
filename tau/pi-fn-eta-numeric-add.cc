#include "pi-fn-eta-prefix.hh"
#include "begin.hh"

namespace τ
{


πdf πηadd()
{
  return πdv(fn {
      τbinfallthrough(+),
      τnbinop(op_add),
      [](πi &i, Stv  a, Stv  b) -> πv { return {St{a} + St{b}}; },
      [](πi &i, St &&a, Stv  b) -> πv { return {a + St{b}}; },
      [](πi &i, Stv  a, St &&b) -> πv { return {St{a} + b}; },
      [](πi &i, St &&a, St &&b) -> πv { return {a + b}; }});
}


}

#include "end.hh"
