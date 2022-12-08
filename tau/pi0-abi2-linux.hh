#ifndef τπ0abi2_linux
#define τπ0abi2_linux


#include "pi0-abi1.hh"


#include "begin.hh"

namespace τ
{


void π0abi2_linux(π0abi &a)
{

}


π0abi const &π0abi2_linux()
{
  static π0abi a;
  static bool  i = false;
  if (!i)
  {
    π0abi1(a);
    π0abi2_linux(a);
  }
  i = true;
  return a;
}


}

#include "end.hh"


#endif
