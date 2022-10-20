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


π0TG π0T(π0h)::~π0h ()
{ A(vs.empty(), "~π0h vs nonempty (will segfault on ~π0hv)");
  A(!rm,        "~π0h during GC");
  for (uN g = 0; g < gn; ++g) delete hs[g]; }


π0TG void π0T(π0h)::mark(π0R x)
{
  if (ms[x.g()]) ms[x.g()]->me(x);
}


π0TG π0R π0T(π0h)::move(π0R x) const
{
  if (rm->contains(x)) return rm->at(x);

  // If the reference wasn't moved as a root, then it's been inlined
  // into something that is now either a root or an inline. We need to
  // reframe x in terms of that object.
  TODO("rewrite inlined references");
}


π0TG void π0T(π0h)::gc(uN s)
{
  gΘ.start();
  ms[0] = new π0T(π0ms){*this, 0};

  for (let v : vs) v->mark();

  rm = new M<π0R, π0R>;
  gs[0] = new π0gs(*ms[0], is);
  B ns;
  ns.reserve(std::max(gs[0]->lss + s << 1, s0));
  for (let r : *gs[0])
    std::cout << "copying " << r << " to " <<
      ((*rm)[r] = π0R(0, ns << π0gso9{*gs[0], r})) << std::endl;

  hs[0]->h = std::move(ns);

  for (let v : vs) v->move();
  delete rm; rm = nullptr;

  for (uN i = 0; i < gn; ++i)
  { delete gs[i]; gs[i] = nullptr;
    delete ms[i]; ms[i] = nullptr; }
  gΘ.stop();
}


}

#include "pi0-gc-end.hh"


#endif
