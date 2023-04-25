#ifndef τηsignal_h
#define τηsignal_h

#include "eta.hh"
#include "xiio.hh"

#include "begin.hh"

namespace τ
{


struct ηexit_ {};
sletc ηexit__ = ηexit_{};  // throw this to terminate a loop

inline void ηexit() { throw ηexit__; }


inline void ηl(ξi i, ξo o, F<void(ηi)> const &f)
{
  try { for (let x : i) if (o) f(x); }
  catch (ηexit_) {}
}


inline void ητ(ξi i, ξo o,
               F<void(ηi)> const &f,
               F<void()>   const &t)
{
  try
  {
    for (let x : i)
      if (x.is_sig() && x.sig() == ηsig::τ) t();
      else if (!o)                          return;
      else                                  f(x);
  }
  catch (ηexit_) {}
}


}

#include "end.hh"

#endif
