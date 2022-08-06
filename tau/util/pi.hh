#ifndef tau_util_pi_h
#define tau_util_pi_h


#include "../types.hh"
#include "../util/numerics.hh"


#include "../module/begin.hh"

namespace tau::util
{


template<class F = u64, class X = u64, uN N = 64>
struct πι_
{
  F n[N]{0};

  πι_ &operator<<(X x) { ++n[numerics::ilog(x)]; return *this; }

  int icdf(double p) const
    { let t = total();
      if (!t) return 0;
      F c  = 0;
      F tc = static_cast<F>(t * p);
      for (uN i = 0; i < N; ++i)
        if ((c += n[i]) > tc)
          return i;
      return N; }

  F total(uN n_ = N) const
    { F t = 0;
      for (uN i = 0; i < n_; ++i) t += n[i];
      return t; }

  πι_ &operator+=(πι_ const &x)
    { for (uN i = 0; i < N; ++i) n[i] += x.n[i];
      return *this; }
};


typedef πι_<> πι;


#if tau_debug_iostream
template<class F, class X, uN N>
O &operator<<(O &s, πι_<F, X, N> const &h)
{
  F   m  = h.n[0]; for (uN i = 1; i < N; ++i) m = std::max(m, h.n[i]);
  uN  u  = N - 1;  while (u > 0 && !h.n[u]) --u;
  let ml = numerics::ilog(m);

  u = std::min(N, u + 7 & ~7);
  if (ml)
    for (uN i = 0; i < u; ++i)
      s << ".123456789abcdef|"[numerics::ilog(h.n[i]) * 16 / ml]
        << (i + 1 & 7 ? "" : " ");

  return s;
}
#endif


}

#include "../module/end.hh"


#endif
