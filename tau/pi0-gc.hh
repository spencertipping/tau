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
  ms = new π0ms{*this, 0, an};
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


inline π0h::~π0h ()
{
  A(vs.empty(), "~π0h vs nonempty (will segfault on ~π0hv)");
  A(!ms,        "~π0h during GC");
  for (uN g = 0; g < gn; ++g) delete hs[g];
}


inline void π0h::mark(π0r x) { if (x) ms->me(x); }
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
  return x && ms->contains(x) ? ms->at(x) : x;
}


inline π0ho9                          *π0h::claim(π0r r, π0r o) { return ms->claim(r, o); }
inline Mo<π0r, π0ho9*>::const_iterator π0h::cb   (π0r r)  const { return ms->cb(r); }
inline Mo<π0r, π0ho9*>::const_iterator π0h::ce   ()       const { return ms->ce(); }


template<O9 T>
π0r π0h::gcw(uN s, T const &x)
{
  B t; t.reserve(s);
  i9 y = t.data() + (t << x);
  π0hnf f{*this, 1};
  f(&y);
  gc(0);
  return y;
}


}

#include "end.hh"


#endif
