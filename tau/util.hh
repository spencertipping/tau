#ifndef tau_util_h
#define tau_util_h


#include "util/hash.hh"
#include "util/pi.hh"
#include "util/shd.hh"
#include "util/nonce.hh"
#include "util/ostream.hh"
#include "util/numerics.hh"


#include "module/begin.hh"

namespace tau
{


using tau::util::nonce;
using tau::util::sha256;


#if tau_debug_iostream || tau_debug_nop
using tau::util::operator<<;
#endif


}

#include "module/end.hh"


#endif
