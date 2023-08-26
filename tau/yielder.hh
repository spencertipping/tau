#ifndef τyielder_h
#define τyielder_h


#include "types.hh"
#include "Lambda.hh"

#include "begin.hh"

namespace τ
{


// Λ yielder: nop that yields after a deadline since the last yield, optionally
// calling a function instead

Txs struct λY final
{
  λY(ΔΘ dt = 10ms, F<void(Xs...)> f = nullptr) : y_(never()), dt_(dt), f_(f) {}

  void operator()(Xs &&... xs)
    { if (y_ == never()) y_ = now();
      if (now() - y_ < dt_) return;
      if (f_) f_(xs...);
      else    Λ_().y(λs::Y);
      y_ = now(); }

  void reset() { y_ = now(); }

protected:
  Θp             y_;   // last yield
  ΔΘ             dt_;  // deadline
  F<void(Xs...)> f_;
};


}

#include "end.hh"


#endif
