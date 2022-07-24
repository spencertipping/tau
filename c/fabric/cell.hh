#ifndef tau_fabric_cell_h
#define tau_fabric_cell_h


#include "../utf9.hh"
#include "../coro.hh"
#include "../species.hh"

#include "either.hh"
#include "monitor.hh"


#include "../begin.hh"


namespace tau::fabric
{

namespace t9 = tau::utf9;
namespace tc = tau::coro;


typedef either<t9::val, t9::val>                 cell_coro_result;
typedef tc::coro<cell_coro_result, cell_monitor> cell_coro;


struct cell
{
  cell_monitor monitor;

  ~cell() {}


};


}


#include "../end.hh"

#endif
