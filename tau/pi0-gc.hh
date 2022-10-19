#ifndef τπ0_gc_h
#define τπ0_gc_h


#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"

#include "pi0-gc-heapspace.hh"
#include "pi0-gc-heap.hh"
#include "pi0-gc-heapview.hh"
#include "pi0-gc-markset.hh"

#include "pi0-gc-begin.hh"

namespace τ
{


π0TG void π0T(π0h)::mark(π0R x)
{
  if (ms[x.g()]) ms[x.g()]->me(x);
}


π0TG π0R π0T(π0h)::move(π0R x) const
{
  TODO("π₀h move()");
}


π0TG void π0T(π0h)::gc(uN s)
{
  gΘ.start();
  ms[0] = new π0T(π0ms){*this, 0};

  for (let v : vs) v->mark();

  auto g = π0gs(std::move(*ms[0]));
  for (let r : g)
    if (g[r])
      TODO("copy ref into newspace");

  // TODO: execute plan

  for (uN i = 0; i < gn; ++i) { delete ms[i]; ms[i] = nullptr; }
  gΘ.stop();
}


}

#include "pi0-gc-end.hh"


#endif
