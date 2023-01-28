#ifndef τγ_h
#define τγ_h


#include "Xi.hh"

#include "begin.hh"

namespace τ
{


// Virtual base for γ compilers
// TODO: add τ reference once tau-epoll is fixed up
struct γ
{
  virtual ~γ() {}
  virtual Ξ operator()(Ξ &x) = 0;
};


}

#include "end.hh"


#endif
