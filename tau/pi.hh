#ifndef tau_pi_h
#define tau_pi_h


#include "types.hh"
#include "numerics.hh"


#include "begin.hh"

namespace tau
{


struct πilog
{
  uN operator()(uN x) { return ilog(x); }
};


template<class F = u64,    // frequency of observation
         class X = u64,    // thing being observed
         uN    N = 64,     // number of bins
         class π = πilog>  // observation → bin
struct πι_
{
  π π_;
  F n[N]{0};

  πι_ &operator<<(X x) { ++n[π_(x)]; return *this; }

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
template<class F, class X, uN N, class π>
O &operator<<(O &s, πι_<F, X, N, π> const &h)
{
  π   π_;
  F   m  = h.n[0]; for (uN i = 1; i < N; ++i) m = std::max(m, h.n[i]);
  uN  u  = N - 1;  while (u > 0 && !h.n[u]) --u;
  let ml = π_(m);

  u = std::min(N, u + 7 & ~7);
  if (ml)
    for (uN i = 0; i < u; ++i)
      s << ".123456789abcdef|"[π_(h.n[i]) * 16 / ml]
        << (i + 1 & 7 ? "" : " ");

  return s;
}
#elif tau_debug_nop
template<class F, class X, uN N, class π>
O &operator<<(O &s, πι_<F, X, N, π> const &h) { return s; }
#endif


}

#include "end.hh"


#endif
