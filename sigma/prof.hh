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
  timer(τ::Sp<measurement> const &m) : start_{τ::now()}, m_{m} {}
  ~timer() { stop(); }

  τ::ΔΘ elapsed() const { return τ::now() - start_; }
  τ::ΔΘ stop();

protected:
  τ::Θp              start_;
  τ::Sp<measurement> m_;
};


struct measurement final
{
  using ΔΘ = τ::ΔΘ;
  using Θp = τ::Θp;
  using uN = τ::uN;
  using iN = τ::iN;

  measurement() : last_(τ::now()) {}

  timer start();
  void  stop(ΔΘ);

  uN n()    const { return n_; }
  ΔΘ min()  const { return min_; }
  ΔΘ max()  const { return max_; }
  ΔΘ mean() const { τ::Sl<τ::Smu> l{mu_}; return n_ ? ΔΘ{virtual_ / n_} : 0ns; }
  ΔΘ gap()  const { τ::Sl<τ::Smu> l{mu_}; return n_ ? ΔΘ{down()   / n_} : 0ns; }

  τ::f64 duty() const { τ::Sl<τ::Smu> l{mu_}; return τ::f64(real().count()) / (real() + down()).count(); }

  uN active() const { return active_; }

  ΔΘ real() const;
  ΔΘ down() const;


protected:
  mutable τ::Smu mu_;                       // for all state
  τ::Wp<measurement> self_;
  uN n_       = 0;                          // number of measurements
  ΔΘ min_     = τ::forever() - τ::never();  // minimum measurement
  ΔΘ max_     = 0ns;                        // maximum measurement
  Θp since_   = τ::never();                 // when the measurement became active
  Θp last_    = τ::never();                 // when the last interval was complete
  iN active_  = 0;                          // active timers
  ΔΘ virtual_ = 0ns;                        // virtual (overlapping) time
  ΔΘ real_    = 0ns;                        // real (non-overlapping) time
  ΔΘ down_    = 0ns;                        // total downtime

  friend τ::Sp<measurement> measurement_for(τ::ηmc&);
};


τ::Sp<measurement> measurement_for(τ::ηmc&);
τ::Sp<measurement> measurement_for(τ::Stc&);
τ::M<τ::ηm, τ::Sp<measurement>> measurements();

timer prof(τ::ηmc&);  // profile an action
timer prof(τ::Stc&);  // profile an action


}

#include "end.hh"

#endif
