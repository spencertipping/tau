#ifndef τ²common_h
#define τ²common_h


#include "types.hh"
#include "Lambda.hh"

#include "begin.hh"

namespace τ
{


struct τΘ
{
  Θp h;
  λi l;

  // NOTE: PQ<ΦΘ> is a maxheap, so < == lower priority == later deadline
  bool operator<(τΘ const &x) const { return h > x.h; }
};


struct τb  // base τ
{
  Λ        l;
  PQ<τΘ>   h;
  Θp const t0 = now();

  τb(τb&)  = delete;
  τb(τb&&) = delete;
  τb() {}


  void Θ(Θp t)    { while (now() < t) { h.push(τΘ{t, l.i()}); l.y(λs::Θ); } }
  Θp   hn() const { return h.empty() ? forever() : h.top().h; }
  ΔΘ   dt() const { return now() - t0; }
};


O &operator<<(O &s, τΘ const &h);


}

#include "end.hh"


#endif
