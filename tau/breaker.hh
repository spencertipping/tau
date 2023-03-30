#ifndef τbreaker_h
#define τbreaker_h


#include "types.hh"
#include "Lambda.hh"

#include "begin.hh"

namespace τ
{


// Λ breaker: catch any attempts to kill the thread and store killed-status
// so we can exit at a safe point. This is used to safely unwind stacks that
// involve exception-unaware C callbacks.
//
// λb(f) → catch trips within f()
// λb.t() → trip the breaker (resuming will unwind)
// λb.r() → resume, unwinding if we have been tripped

struct λb final
{
  // Execute a function safely, that is, catching all trips that may result.
  // We store the trip state on this object but don't unwind beyond this point.
  void operator()(F<void()> const &f)
    { if (!*this)
        try         { f(); }
        catch (Λx_) { t(); } }

  operator bool() const { return t_; }

  λb  &t()       { t_ = true; return *this; }
  void r() const { if (*this) throw λx_; }


protected:
  bool t_ = false;  // trip state
};


}

#include "end.hh"


#endif
