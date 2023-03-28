#ifndef τ²common_h
#define τ²common_h


#include "types.hh"
#include "Lambda.hh"
#include "xiio.hh"

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
  τb(τb&)  = delete;
  τb(τb&&) = delete;
  τb() {}

  Λ  &l()      { return l_; }
  ΔΘ  dt(Θp p) { return t0_ - p; }


  void Θ(Θp t)    { while (now() < t) { h_.push(τΘ{t, l_.i()}); l_.y(λs::Θ); } }
  Θp   hn() const { return h_.empty() ? forever() : h_.top().h; }
  ΔΘ   dt() const { return now() - t0_; }


  P<ξo, ξi> pipe(uN c = 8192)
    { let x = Sp<ξ>(new ξ(l_, c));
      return mp(ξo(x), ξi(x)); }


protected:
  Λ        l_;
  PQ<τΘ>   h_;
  Θp const t0_ = now();
};


O &operator<<(O &s, τΘ const &h);


}

#include "end.hh"


#endif
