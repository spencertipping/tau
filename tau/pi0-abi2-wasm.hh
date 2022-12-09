#ifndef τπ0abi2_wasm
#define τπ0abi2_wasm


#include "pi0-abi1.hh"


#include "begin.hh"

namespace τ
{


void π0abi2_wasm(π0abi &a)
{

}


π0abi const &π0abi2_wasm()
{
  static π0abi a;
  static bool  i = false;
  if (!i) π0abi1(a), π0abi2_wasm(a);
  i = true;
  return a;
}


}

#include "end.hh"


#endif
