#ifndef τπ0_gc_h
#define τπ0_gc_h


#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"

#include "pi0-gc-heapspace.hh"
#include "pi0-gc-heap.hh"
#include "pi0-gc-heapview.hh"
#include "pi0-gc-markset.hh"

#include "begin.hh"

namespace τ
{


inline π0h::~π0h ()
{
  A(vs.empty(), "~π0h vs nonempty (will segfault on ~π0hv)");
  A(!ms,        "~π0h during GC");
  for (uN g = 0; g < gn; ++g) delete hs[g];
}


inline void π0h::mark(π0r x)
{
  // Mark the object as having an external reference.
  ms->mark(x);
}


inline π0r π0h::move(π0r x) const
{
  // If the object was never marked, then it's uninvolved in this GC.
  // This happens if we're just collecting newgen and the reference
  // is oldgen, for example.
  //
  // Otherwise, we should have an entry for the reference in the
  // move table.
  return ms->contains(x) ? ms->at(x) : x;
}


inline void π0h::move(π0r f, π0r t)
{
  // NOTE: this function may be called multiple times for the same
  // source ref, e.g. if the reference is inlined in several places.
  //
  // TODO: is this true?
  (ms->n)[f] = t;
}


void π0h::gc(uN s)
{
  A(!ms, "gc() within GC");
  gΘ.start();
  ms = new π0ms{*this};
  for (let v : vs) v->mark();
  ms->gc();
  for (let v : vs) v->move();
  delete ms; ms = nullptr;
  gΘ.stop();
}


}

#include "end.hh"


#endif
