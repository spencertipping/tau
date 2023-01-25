#ifndef τΣ_h
#define τΣ_h


#include "types.hh"
#include "numerics.hh"


#include "begin.hh"

namespace τ
{


struct Σilog
{
  uN operator()(uN x) { return ilog(x); }
};


template<class F = u64,    // frequency of observation
         class X = u64,    // thing being observed
         uN    N = 64,     // number of bins
         class π = Σilog>  // observation → bin
struct Σι_
{
  π π_;
  F n[N]{0};

  Σι_ &operator<<(X x) { ++n[π_(x)]; return *this; }

  uN icdf(double p) const
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

  Σι_ &operator+=(Σι_ const &x)
    { for (uN i = 0; i < N; ++i) n[i] += x.n[i];
      return *this; }
};


typedef Σι_<> Σι;


struct Σ0
{
  template<class X>
  Σ0 &operator<<(X)             { return *this; }

  uN  icdf      (double)  const { return 0; }
  u64 total     (uN = 64) const { return 0; }
  Σ0 &operator+=(Σ0)            { return *this; }
};


#if τdebug_iostream
template<class F, class X, uN N, class π>
O &operator<<(O &s, Σι_<F, X, N, π> const &h)
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
#endif


}

#include "end.hh"


#endif
