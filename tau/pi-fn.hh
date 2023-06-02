#ifndef τπfn_h
#define τπfn_h

#include "pi-int.hh"
#include "begin.hh"

namespace τ
{


// A π function that can act on an interpreter, and which adjusts the
// stack size by N.
//
// TODO: think about the representation; we should be able to flatten the
// function down to a series of C++ fn pointers or std::function objects,
// and it's not entirely clear that virtual objects are necessary.
template<iN N>
struct πf
{
  virtual ~πf() {}
  virtual void operator()(πi&) const = 0;
};


}

#include "end.hh"

#endif
