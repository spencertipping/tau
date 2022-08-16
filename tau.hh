#ifndef tau_h
#define tau_h


#include "tau/arch.hh"
#include "tau/debug.hh"

#include "tau/types.hh"
#include "tau/utf9.hh"
#include "tau/flux.hh"

#include "tau/util/hash.hh"
#include "tau/util/pi.hh"
#include "tau/util/shd.hh"
#include "tau/util/nonce.hh"
#include "tau/util/ostream.hh"
#include "tau/util/numerics.hh"


#include "tau/module/begin.hh"

namespace tau
{


#if tau_debug_iostream || tau_debug_nop
using tau::util::operator<<;
#endif


}

#include "tau/module/end.hh"


#endif
