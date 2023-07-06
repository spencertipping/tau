#include "shd.hh"
#include "begin.hh"

namespace τ
{

using namespace std::chrono;


O &operator<<(O &s, ΔΘ const &t)
{
  let a = abs(t);
  if      (a <= 100us)   return s << t.count() << "ns";
  else if (a <= 100ms)   return s << t / 1us   << "μs";
  else if (a <= 100s)    return s << t / 1ms   << "ms";
  else if (a <= 1h)      return s << t / 1s    << "s";
  else if (a <= 10h)     return s << t / 1min  << "m";
  else if (a <= 168h)    return s << t / 1h    << "h";
  else if (a <= 24h*730) return s << duration_cast<days>(t).count() << "d";
  else                   return s << duration_cast<years>(t).count() << "y";
  return s;
}

O &operator<<(O &s, Θp const &p)
{
  return s << "t+" << p - now();
}

O &operator<<(O &s, ΣΘΔ const &w)
{
  return s << "ΣΘΔ[" << (w.Θr ? "R" : "s")
           << " n="  << w.n
           << " Σ="  << w.Σ()
           << " μ="  << w.μ()
           << " 50=" << w.p(0.5)
           << " 90=" << w.p(0.9)
           << " 99=" << w.p(0.99) << "]";
}

O &operator<<(O &s, ΘΔ const &w)
{
  return s << "ΘΔ[" << (w.Θr ? "R" : "s")
           << " n=" << w.n
           << " Σ=" << w.Σ()
           << " μ=" << w.μ() << "]";
}


}

#include "end.hh"
