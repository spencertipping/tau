#ifndef tau_util_ostream_h
#define tau_util_ostream_h


#include <iostream>

#include "../types.hh"

#include "../module/begin.hh"


namespace tau::util
{


inline O &hexout(O &s, u64 x)
{
  let f = s.flags();
  s << std::hex << std::nouppercase << x;
  s.flags(f);
  return s;
}


}


#include "../module/end.hh"

#endif
