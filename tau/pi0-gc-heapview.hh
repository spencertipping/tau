#ifndef τπ0_gc_heapview_h
#define τπ0_gc_heapview_h


#include "types.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"

#include "pi0-gc-begin.hh"

namespace τ
{


π0TGs π0hv  // heap view: a part of the root set
{
  π0TS;
  π0T(π0h) &h;
  π0hv(π0T(π0h) &h_) : h(h_) {}
  virtual ~π0hv() { h.vs.erase(this); }
  virtual void mark() = 0;  // mark everything in this view
  virtual void move() = 0;  // move everything in this view
};


// TODO: stack view
// TODO: frame view


π0TGs π0hnf : virtual π0hv  // native frame heap view
{
  π0TS;
  V<π0T(π0r)> v;

  π0hnf()             = delete;
  π0hnf(π0hnf const&) = delete;
  π0hnf(π0hnf&&)      = delete;
  π0hnf(π0T(π0h) &h_, uN vs) : π0hv(h_) { v.reserve(vs); }

  void mark() { for (let   x : v) h.mark(x); }
  void move() { for (auto &x : v) x = h.move(x); }

  π0T(π0r) &operator<<(π0T(π0r) x)
  { A(v.size() + 1 <= v.capacity(),
      "π₀hnf: undersized frame (reserved " << v.capacity() << ")");
    v.push_back(x);
    return v.back(); }
};


}

#include "pi0-gc-end.hh"


#endif
