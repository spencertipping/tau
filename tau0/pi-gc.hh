#ifndef τπ_gc_h
#define τπ_gc_h


#include "types.hh"
#include "utf9.hh"
#include "pi-types.hh"

#include "pi-gc-heapspace.hh"
#include "pi-gc-heap.hh"
#include "pi-gc-heapview.hh"
#include "pi-gc-markset.hh"

#include "begin.hh"

namespace τ
{


void πh::gc(uN s)
{
  A(!ms, "gc() within GC");

  gΘ.start();
  ms = new πms{*this, 0, an};
  an = 0;
  for (let v : vs) v->mark();

  // TODO: replace this temporary logic with real generational GC
  let lss = note_lss(ms->plan());
  B h_; h_.reserve(std::max(this->s[0], lss + s << 2));
  *ms >> h_;
  A(h_.size() == lss,
    "GC live-set size mismatch: " << h_.size() << " ≠ " << lss);
  hs[0]->h.swap(h_);

  for (let v : vs) v->move();
  delete ms; ms = nullptr;
  gΘ.stop();
}


inline πh::~πh ()
{
  A(vs.empty(), "~πh vs nonempty (will segfault on ~πhv)");
  A(!ms,        "~πh during GC");
  for (uN g = 0; g < gn; ++g) delete hs[g];
}


inline void πh::mark(πr x) { if (x) ms->me(x); }
inline πr   πh::move(πr x) const
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
  return x && ms->contains(x) ? ms->at(x) : x;
}


inline πho9                         *πh::claim(πr r, πr o) { return ms->claim(r, o); }
inline Mo<πr, πho9*>::const_iterator πh::cb   (πr r) const { return ms->cb(r); }
inline Mo<πr, πho9*>::const_iterator πh::ce   ()     const { return ms->ce(); }


template<O9 T>
πr πh::gcw(uN s, T const &x)
{
  B t; t.reserve(s);
  i9 y = t.data() + (t << x);
  πhnf f{*this, 1};
  f(&y);
  gc(0);
  return y;
}


}

#include "end.hh"


#endif
