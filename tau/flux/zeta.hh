#ifndef tau_flux_zeta_h
#define tau_flux_zeta_h


#include "../types.hh"

#include "Lambda.hh"
#include "zetab.hh"


#include "../module/begin.hh"

namespace tau::flux
{


ζi const constexpr ζib = 16;     // maximum #bits to refer to ζ
uN const constexpr ζc0 = 65536;  // default capacity

void* ζs[1 << ζib] = {nullptr};


// TODO: reserve capacity blocks by c
ζi ζni(uN c, void *x)
{
  for (ζi i = 0; i < 1 << ζib; ++i)
    if (ζs[i] == nullptr)
    {
      ζs[i] = x;
      return i;
    }
  return -1;
}


template<class T>
struct ζr;

template<class T, class RC = u8>
struct ζ
{
  Λ        &l;
  ζi const  i;
  ζb        b;

  ζ(Λ &l_, uN c_ = ζc0) : l(l_), i(ζni(c_, this)), b(c_ * 2) {}
  ~ζ() { ζs[i] = nullptr; }

  RC   rc(uN a) const { return R<RC>(b + a, 0); }
  void rc(uN a, RC x) {        W<RC>(b + a, 0, x); }

  ζ &claim  (uN a) { rc(a, rc(a) + 1); return *this; }
  ζ &unclaim(uN a) { rc(a, rc(a) - 1); return *this; }
};


template<class T>
struct ζr
{
  uNc i;
  ζr(uN i_) : i(i_) { z().claim  (xi()); }
  ~ζr()             { z().unclaim(xi()); }

#if tau_wordsize == 32
  ζi zi() const {}
  uN xi() const {}
#elif tau_wordsize == 64
  ζi zi() const {}
  uN xi() const {}
#else
# error invalid word size for ζr
#endif

  ζ<T> &z() const { return *Rc<ζ<T>*>(ζs[zi()]); }
};


}

#include "../module/end.hh"


#endif
