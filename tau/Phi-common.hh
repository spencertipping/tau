#ifndef τΦcommon_h
#define τΦcommon_h


#include "types.hh"
#include "Lambda.hh"

#include "begin.hh"

namespace τ
{


struct ΦΘ
{
  Θp h;
  λi l;
  bool operator<(ΦΘ const &x) const { return h > x.h; }
};


struct Φb  // base Φ
{
  Λ        l;
  PQ<ΦΘ>   h;
  Θp const t0 = now();

  Φb(Φb&)  = delete;
  Φb(Φb&&) = delete;
  Φb() {}


  void Θ(Θp t)    { while (now() < t) { h.push(ΦΘ{t, l.i()}); l.y(λs::Θ); } }
  Θp   hn() const { return h.empty() ? forever() : h.top().h; }
  ΔΘ   dt() const { return now() - t0; }
};


}

#include "end.hh"


#endif
