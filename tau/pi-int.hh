#ifndef τπint_h
#define τπint_h


#include "pi-val.hh"
#include "xiio.hh"
#include "tau.hh"

#include "begin.hh"

namespace τ
{


struct πi
{
  ξi        i;   // input stream (if streaming)
  ξo        o;   // output stream (if streaming)
  ηm        m;   // buffered input value (if not streaming)
  M<St, πv> vs;  // stored variables

  πi()                            {}
  πi(ηic &x)       : m(x)         {}
  πi(ηm &&m_)      : m(mo(m_))    {}
  πi(ηm  &m_)      : m(m_)        {}
  πi(ξi i_, ξo o_) : i(i_), o(o_) {}


  ηi          y() const {        return m.empty() ? *i : m.y(); }
  πi &operator=(ηic &x) { m = x; return *this; }

  [[noreturn]] void fail(St reason) { throw reason; }
};


}

#include "end.hh"


#endif
