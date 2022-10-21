#ifndef τπ0_gc_heapview_h
#define τπ0_gc_heapview_h


#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"

#include "pi0-gc-begin.hh"

namespace τ
{


π0TGs π0hv  // heap view: a part of the root set
{
  π0TS;
  π0T(π0h) &h;
  π0hv(π0T(π0h) &h_) : h(h_) { h.vs.insert(this); }
  virtual ~π0hv()            { h.vs.erase(this); }
  virtual void mark() = 0;  // mark everything in this view
  virtual void move() = 0;  // move everything in this view
};


π0TGs π0hds : virtual π0T(π0hv)  // data-stack heap view
{
  π0TS;
  V<π0R> s;

  π0hds()             = delete;
  π0hds(π0hds const&) = delete;
  π0hds(π0hds&&)      = delete;
  π0hds(π0T(π0h) &h_) : π0T(π0hv)(h_) {}

  void mark() { for (let   x : s)     π0T(π0hv)::h.mark(x); }
  void move() { for (auto &x : s) x = π0T(π0hv)::h.move(x); }

  π0hds      &operator<<(π0R x)         { s.push_back(x); return *this; }
  π0TO π0hds &operator<<(π0To const &x) { return *this << (π0T(π0hv)::h << x); }

  π0R operator[](uN i) const { return s.at(s.size() - i - 1); }
  π0R        pop()           { let r = s.back(); s.pop_back(); return r; }
  π0hds    &drop(uN n = 1)   { s.resize(s.size() - n); return *this; }
  uN        size()     const { return s.size(); }
};


π0TGs π0hdf : virtual π0T(π0hv)  // stack-of-frames heap view
{
  π0TS;
  V<π0R> d;  // all frames concatenated together
  V<uN>  n;  // first variable within each frame

  π0hdf()             = delete;
  π0hdf(π0hdf const&) = delete;
  π0hdf(π0hdf&&)      = delete;
  π0hdf(π0T(π0h) &h_) : π0T(π0hv)(h_) {}

  void mark() { for (let   x : d)     π0T(π0hv)::h.mark(x); }
  void move() { for (auto &x : d) x = π0T(π0hv)::h.move(x); }

  π0R &operator[](uN i) { return d[n.back() + i]; }

  π0hdf &push(uN s) { n.push_back(d.size()); d.resize(d.size() + s, π0R()); return *this; }
  π0hdf  &pop()     { d.resize(n.back()); n.pop_back(); return *this; }
};


π0TGs π0hnf : virtual π0T(π0hv)  // native frame heap view
{
  π0TS;
  V<π0R> v;

  π0hnf()             = delete;
  π0hnf(π0hnf const&) = delete;
  π0hnf(π0hnf&&)      = delete;
  π0hnf(π0T(π0h) &h_, uN vs) : π0T(π0hv)(h_) { v.reserve(vs); }

  void mark() { for (let   x : v)     π0T(π0hv)::h.mark(x); }
  void move() { for (auto &x : v) x = π0T(π0hv)::h.move(x); }

  π0R &operator<<(π0R x)
  { A(v.size() + 1 <= v.capacity(),
      "π₀hnf: undersized frame (reserved " << v.capacity() << ")");
    v.push_back(x);
    return v.back(); }
};


}

#include "pi0-gc-end.hh"


#endif
