#ifndef τπabi2_linux
#define τπabi2_linux


#include "pi-abi1.hh"


#include "begin.hh"

namespace τ
{


void πabi2_syscalls(πabi &a)
{

}


void πabi2_linux(πabi &a)
{

}


πabi const &πabi2_linux()
{
  static πabi a;
  static bool i = false;
  if (!i) πabi1(a), πabi2_linux(a);
  i = true;
  return a;
}


}

#include "end.hh"


#endif
