#ifndef tau_fabric_cell_h
#define tau_fabric_cell_h


#include <functional>


#include "../utf9.hh"
#include "../coro.hh"
#include "../species.hh"

#include "scheduler.hh"


#include "../begin.hh"


namespace tau::fabric
{

namespace ts = tau::species;


struct cell
{
  ts::species const &species;


  cell(ts::species const &species_)
    : species(species_)
    {
    }

  ~cell() {}

  bool operator==(cell &x) const { return this == &x; }
};


}


template<> struct std::hash<tau::fabric::cell>
{
  static std::hash<void const*> h;
  uint64_t operator()(tau::fabric::cell const &c) const
    { return h(&c); }
};


#include "../end.hh"

#endif
