#ifndef τπ0_gc_h
#define τπ0_gc_h


#include "Sigma.hh"
#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc-splice.hh"
#include "pi0-gc-heapspace.hh"
#include "pi0-gc-heap.hh"
#include "pi0-gc-heapview.hh"
#include "pi0-gc-markset.hh"

#include "pi0-gc-begin.hh"

namespace τ
{


π0TG void π0T(π0h)::mark(π0T(π0r) x)
{
  if (ms[x.g()]) ms[x.g()]->mark_external(x);
}


π0TG π0T(π0r) π0T(π0h)::move(π0T(π0r) x) const
{
  TODO("π₀h move()");
}


π0TG void π0T(π0h)::gc(uN g, uN s)
{
  for (uN g = 0; g < gn; ++g)
  { ms[g] = new π0T(π0ms){*this};
    ss[g] = new π0T(π0gs){g, gs[g]->h}; }

  for (let v : vs) v->mark();

  // TODO: execute plan

  for (uN i = 0; i < gn; ++i)
  { if (ms[g]) delete ms[g];
    if (ss[g]) delete ss[g];
    ms[g] = nullptr;
    ss[g] = nullptr; }
}


}

#include "pi0-gc-end.hh"


#endif
