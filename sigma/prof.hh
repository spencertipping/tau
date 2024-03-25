#ifndef σprof_h
#define σprof_h

#include "begin.hh"

namespace σ
{

using namespace std::literals;


void Γprof(Γφ&);


struct measurement;


struct timer final
{
  timer(measurement &m) : m_{&m}, start_{τ::now()} {}
  ~timer() { stop(); }

  τ::ΔΘ stop();

protected:
  measurement *m_;
  τ::Θp           start_;
};


struct measurement final
{
  using ΔΘ = τ::ΔΘ;
  using Θp = τ::Θp;
  using uN = τ::uN;
  using iN = τ::iN;

  timer start();
  void  stop(ΔΘ);

  uN n()    const { return n_; }
  ΔΘ min()  const { return min_; }
  ΔΘ max()  const { return max_; }
  ΔΘ mean() const { τ::Sl<τ::Smu> l{mu_}; return n_ ? ΔΘ{virtual_ / n_} : 0ns; }

  uN active() const { return active_; }

  ΔΘ real() const;


protected:
  mutable τ::Smu mu_;                       // for all state
  uN n_       = 0;                          // number of measurements
  ΔΘ min_     = τ::forever() - τ::never();  // minimum measurement
  ΔΘ max_     = 0ns;                        // maximum measurement
  Θp since_   = τ::never();                 // when the measurement became active
  iN active_  = 0;                    // active timers
  ΔΘ virtual_ = 0ns;                  // virtual (overlapping) time
  ΔΘ real_    = 0ns;                  // real (non-overlapping) time
};


measurement &measurement_for(τ::ηmc&);
measurement &measurement_for(τ::Stc&);

timer prof(τ::ηmc&);  // profile an action
timer prof(τ::Stc&);  // profile an action


}

#include "end.hh"

#endif
