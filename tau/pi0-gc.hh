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


void π0h::gc(uN s)
{
  A(!ms, "gc() within GC");

  gΘ.start();
  ms = new π0ms{*this};
  for (let v : vs) v->mark();

  // TODO: replace this temporary logic with real generational GC
  let lss = ms->plan();
  B h_; h_.reserve(lss + s << 2);
  *ms >> h_;
  hs[0]->h.swap(h_);

  for (let v : vs) v->move();
  delete ms; ms = nullptr;
  gΘ.stop();
}


inline π0h::~π0h ()
{
  A(vs.empty(), "~π0h vs nonempty (will segfault on ~π0hv)");
  A(!ms,        "~π0h during GC");
  for (uN g = 0; g < gn; ++g) delete hs[g];
}


inline void π0h::mark(π0r x) { ms->me(x); }
inline π0r  π0h::move(π0r x) const
{
  // If the object was never marked, then it's uninvolved in this GC.
  // This happens if we're just collecting newgen and the reference
  // is oldgen, for example.
  //
  // Otherwise, we should have an entry for the reference in the
  // move table.
  //
  // NOTE: this function's results are valid only if you mark all
  // references that you then ask about.
  return ms->contains(x) ? ms->at(x) : x;
}


inline π0ho9                          *π0h::claim(π0r r, π0r o) { return ms->claim(r, o); }
inline Mo<π0r, π0ho9*>::const_iterator π0h::cb   (π0r r)  const { return ms->cb(r); }
inline Mo<π0r, π0ho9*>::const_iterator π0h::ce   ()       const { return ms->ce(); }


}

#include "end.hh"


#endif
