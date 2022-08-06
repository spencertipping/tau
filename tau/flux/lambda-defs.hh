#ifndef tau_flux_λ_defs_h
#define tau_flux_λ_defs_h


#include <iostream>


#include "../types.hh"
#include "../util/shd.hh"

#include "init.hh"
#include "defs.hh"


#include "../module/begin.hh"

namespace tau::flux
{


enum λs : u8
{
  RUNNABLE,
  SLEEPING,
  READ_BLOCKED,
  WRITE_BLOCKED,
  DONE
};


// TODO: oblierate this; we'll split it across multiple facet-managers
struct λ
{
  ΣΘΔ     monitor;
  λc      coro;
  ΣΘΔ::Θp deadline;
  λs      state;
  ψi      blocker;

  λ() {}
  λ(λf const &f) : coro(λc(f)), state{RUNNABLE} {}

  λ &operator=(λ &&t)  // NOTE: required so we can use these in maps
    { monitor  = t.monitor;
      coro     = std::move(t.coro);
      deadline = t.deadline;
      state    = t.state;
      blocker  = t.blocker;
      return *this; }

  ΔΘ p(double p) const { return monitor.p(p); }

  bool operator<(λ const &t) const;
};


std::ostream &operator<<(std::ostream &s, λs const &t)
{
  switch (t)
  {
  case RUNNABLE:      return s << "R";
  case SLEEPING:      return s << "S";
  case READ_BLOCKED:  return s << "I";
  case WRITE_BLOCKED: return s << "O";
  case DONE:          return s << "Z";
  default:            return s << "BOGUS " << Sc<u8>(t);
  }
}


std::ostream &operator<<(std::ostream &s, λ const &t)
{
  s << "λ[" << t.state;
  if (t.state == SLEEPING) s << " d=t+" << (t.deadline - ΣΘΔ::now());
  else if (t.state == READ_BLOCKED || t.state == WRITE_BLOCKED) s << t.blocker;
  return s << " " << t.monitor << "]";
}


}

#include "../module/end.hh"


#endif
