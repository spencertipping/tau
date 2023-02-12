#ifndef τπint_h
#define τπint_h

#include "types.hh"
#include "eta.hh"
#include "xiio.hh"

#include "begin.hh"

namespace τ
{


struct πi;

template<class T> using πf = F<T(πi&)>;


struct πv
{

};


struct πi
{
  πi() {}
  πi(ξi i__, ξo o__) : i_(i__), o_(o__) {}

  η    y () const { return *i_; }
  η0o &ηo()       { return os_.back(); }


protected:
  ξi     i_;
  ξo     o_;
  V<η0o> os_;
};


}

#include "end.hh"


#endif
