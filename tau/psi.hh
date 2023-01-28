#ifndef τψ_h
#define τψ_h


#include "lambda-types.hh"
#include "Lambda.hh"
#include "begin.hh"

namespace τ
{


struct ψ
{
  virtual ~ψ() {}

  virtual St name() const = 0;

protected:
  S<λi> ls;
};


}

#include "end.hh"


#endif
