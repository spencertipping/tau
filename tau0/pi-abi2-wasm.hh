#ifndef τπabi2_wasm
#define τπabi2_wasm


#include "pi-abi1.hh"


#include "begin.hh"

namespace τ
{


void πabi2_wasm(πabi &a)
{

}


πabi const &πabi2_wasm()
{
  static πabi a;
  static bool i = false;
  if (!i) πabi1(a), πabi2_wasm(a);
  i = true;
  return a;
}


}

#include "end.hh"


#endif
