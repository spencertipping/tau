#ifndef tau_h
#define tau_h


#include "tau/utf9.hh"
#include "tau/species.hh"

#include "tau/kern/coro.hh"
#include "tau/kern/cell.hh"
#include "tau/kern/leaf.hh"
#include "tau/kern/log-histogram.hh"
#include "tau/kern/scheduler.hh"
#include "tau/kern/stopwatch.hh"


#include "tau/module/begin.hh"

namespace tau
{

using kern::stopwatch;
using kern::operator<<;

}

#include "tau/module/end.hh"


#endif
