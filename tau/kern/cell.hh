#ifndef tau_fabric_cell_h
#define tau_fabric_cell_h


#include <functional>


#include "../utf9.hh"
#include "../species.hh"

#include "coro.hh"

#include "scheduler.hh"


#include "../module/begin.hh"


namespace tau::kern
{


struct cell
{
  species const &sp;

  cell(species const &species_)
    : sp(species_)
    {}

  ~cell() {}

  bool operator==(cell &x) const { return this == &x; }
};


}


template<> struct std::hash<tau::kern::cell>
{
  static std::hash<void const*> h;
  uint64_t operator()(tau::kern::cell const &c) const
    { return h(&c); }
};


#include "../module/end.hh"

#endif
