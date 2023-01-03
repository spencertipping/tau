#ifndef τΦcommon_h
#define τΦcommon_h


#include "types.hh"
#include "Lambda.hh"
#include "pi-gc.hh"

#include "types.hh"
#include "Lambda.hh"
#include "phi.hh"
#include "utf9-i9.hh"
#include "utf9-o9.hh"
#include "phi9.hh"

#include "begin.hh"

namespace τ
{


typedef φ_<i9, i9, φ9> φ;

template<class T>
typename std::enable_if<iti<T>::v, bool>::type
operator>(T &x, φ &y)
{
  A(y.l.i(), "iti > φ on λi = 0");
  for (let a : x) if (!(y << a)) return false;
  return true;
}

template<class T>
typename std::enable_if<iti<T>::v, bool>::type
operator<(φ &y, T &x)
{
  return x > y;
}

template<class T>
typename std::enable_if<iti<T>::v, bool>::type
operator<(φ &y, T &&x)
{
  return x > y;
}


struct Φ;


struct ΦΘ
{
  Θp h;
  λi l;

  // NOTE: PQ<ΦΘ> is a maxheap, so < == lower priority == later deadline
  bool operator<(ΦΘ const &x) const { return h > x.h; }
};


struct Φb  // base Φ
{
  Λ        l;
  PQ<ΦΘ>   h;
  Θp const t0 = now();
  πh       ph{64, {65536, 0}};

  Φb(Φb&)  = delete;
  Φb(Φb&&) = delete;
  Φb() {}


  void Θ(Θp t)    { while (now() < t) { h.push(ΦΘ{t, l.i()}); l.y(λs::Θ); } }
  Θp   hn() const { return h.empty() ? forever() : h.top().h; }
  ΔΘ   dt() const { return now() - t0; }
};


#if τdebug_iostream
template<class T>
O &operator<<(O &s, φ9::φo<T> const &o)
{
  s << "φo ";
  if (o.x.index() == 0) s << std::get<0>(o.x);
  else                  s << std::get<1>(o.x);
  return s;
}

O &operator<<(O &s, φ9 const &f)
{
  return s << "φ9[" << f.im << " " << f.om << "+" << f.ohc
           << "^" << f.ov << (f.b ? " H" : "") << "]";
}
#endif


}

#include "end.hh"


#endif
