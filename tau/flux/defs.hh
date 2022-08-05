#ifndef tau_flux_defs_h
#define tau_flux_defs_h


#include <chrono>
#include <iostream>


#include "../types.hh"
#include "../utf9.hh"

#include "init.hh"
#include "coro.hh"
#include "pipe.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct Ψ;  // unit boundary
struct Λ;  // flux core
struct Φ;  // flux manager
struct λ;  // unit coro


typedef uN λi;
typedef uN ψi;
typedef uN Ψi;


// FIXME: task results should be something more interesting, probably
typedef int                 λv;
typedef coro::coro<λv>      λc;
typedef std::function<λv()> λf;

typedef u9                  ψv;  // FIXME: shared_ptr, ibuf handling
typedef pipe<ψv>            ψ;


struct deadline_schedule
{
  Λ &c;
  bool operator()(λi a, λi b) const;
};


}

#include "../module/end.hh"


#endif
