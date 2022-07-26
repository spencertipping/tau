#ifndef tau_fabric_leaf_h
#define tau_fabric_leaf_h


#include "species.hh"

#include "cell.hh"
#include "scheduler.hh"


#include "module/begin.hh"


namespace tau
{


struct leaf
{
  species const &sp;
  sc             sched;
};


}


#include "module/end.hh"

#endif
