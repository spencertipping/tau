#ifndef τηsignal_h
#define τηsignal_h

#include "eta.hh"
#include "Psi.hh"
#include "xiio.hh"

#include "begin.hh"

namespace τ
{


inline void ηl(ξi i, ξo o, F<void(ηi)> const &f)
{
  for (let x : i) if (o) f(x);
}


inline void ητ(ξi i, ξo o,
               F<void(ηi)> const &f,
               F<void()>   const &t)
{
  for (let x : i)
    if (x.is_sig() && x.sig() == ηsig::τ) t();
    else if (!o)                          return;
    else                                  f(x);
}


}

#include "end.hh"

#endif
