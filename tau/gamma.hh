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

  virtual St name() const = 0;
};


O &operator<<(O&, γ const&);


}

#include "end.hh"


#endif
