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


π0h::~π0h ()
{ A(vs.empty(), "~π0h vs nonempty (will segfault on ~π0hv)");
  A(!rm,        "~π0h during GC");
  for (uN g = 0; g < gn; ++g) delete hs[g]; }


void π0h::mark(π0r x)
{
  // Mark the object as having an external reference.
  ms->me(x);
}


π0r π0h::move(π0r x) const
{
  // If the object was never marked, then it's uninvolved in this GC.
  // This happens if we're just collecting newgen and the reference
  // is oldgen, for example.
  //
  // Otherwise, we should have an entry for the reference in the
  // move table.
  return rm->contains(x) ? rm->at(x) : x;
}


void π0h::gc(uN s)
{
  gΘ.start();

  // TODO: rewrite this function
  /*

  ms[0] = new π0ms{*this, 0};

  // TODO: tenure stuff to older generations

  for (let v : vs) v->mark();

  rm = new M<π0R, π0R>;
  gs[0] = new π0gs(*ms[0], is);

  B ns;
  ns.reserve(std::max(gs[0]->lss + s << 1, s0));
  for (let r : *gs[0])
    (*rm)[r] = π0R(0, ns << π0T(π0gso9){*gs[0], r});

  hs[0]->h = std::move(ns);

  for (let v : vs) v->move();
  delete rm; rm = nullptr;

  for (uN i = 0; i < gn; ++i)
  { delete gs[i]; gs[i] = nullptr;
    delete ms[i]; ms[i] = nullptr; }
  */

  gΘ.stop();
}


}

#include "end.hh"


#endif
