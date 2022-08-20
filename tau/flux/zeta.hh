#ifndef tau_flux_zeta_h
#define tau_flux_zeta_h


#include <memory>


#include "../types.hh"
#include "Lambda.hh"


#include "../module/begin.hh"

namespace tau::flux
{


ζi const constexpr ζib = 16;     // maximum #bits to refer to ζ
uN const constexpr ζpb = 10;     // refcount page bits
uN const constexpr ζc0 = 65536;  // default capacity

typedef u8 ζrc;                  // refcount container

void* ζs[1 << ζib] = {0};


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


template<class T>
struct ζ
{
  Λ        &l;
  ζi const  i;
  uN        c;
  u8       *xs = nullptr;
  ζrc      *rc = nullptr;
  uN        ri = 0;
  uN        wi = 0;

  ζ(Λ &l_, uN c_ = ζc0) : l(l_), i(ζni(c_, this)), c(c_)
    { xs = Rc<u8*>(std::malloc(c_ & ~(-1ull << ζpb)));
      rc = Rc<u8*>(std::calloc(c_ >> ζpb, sizeof(u8))); }

  ~ζ() { std::free(xs); std::free(rc); }
};


template<class T>
struct ζr
{
  uN const i;
  ζr(uN i_) : i(i_)
    {
    }

#if tau_wordsize == 32
  ζi zi() const {}
  uN xi() const {}
#elif tau_wordsize == 64
  ζi zi() const {}
  uN xi() const {}
#else
# error invalid word size for ζr
#endif
};


}

#include "../module/end.hh"


#endif
