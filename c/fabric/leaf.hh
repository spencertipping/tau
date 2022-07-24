#ifndef tau_fabric_leaf_h
#define tau_fabric_leaf_h


#include "../species.hh"

#include "cell.hh"
#include "scheduler.hh"


#include "../begin.hh"


namespace tau::fabric
{

namespace ts = tau::species;


struct leaf
{
  ts::species const &species;
  scheduler          sched;
};


}


#include "../end.hh"

#endif
