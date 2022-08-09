#ifndef tau_util_nonce_h
#define tau_util_nonce_h


#include "../types.hh"
#include "ostream.hh"

#include "../module/begin.hh"

namespace tau::util
{


struct nonce
{
  u64 h{0}, l{0};

  nonce &operator++()                     { if (!++l) ++h; return *this; }
  bool   operator==(nonce const &n) const { return l == n.l && h == n.h; }
};


#ifdef tau_debug_iostream
O &operator<<(O &s, nonce const &n)
{
  return s << n.h << ":" << n.l;
}
#endif


}

#include "../module/end.hh"


#endif
