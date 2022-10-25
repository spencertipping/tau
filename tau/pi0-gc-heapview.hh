#ifndef τπ0_gc_heapview_h
#define τπ0_gc_heapview_h


#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"

#include "begin.hh"

namespace τ
{


struct π0hv  // heap view: a part of the root set
{
  π0h &h;
  π0hv(π0h &h_) : h(h_) { h.vs.insert(this); }
  virtual ~π0hv()       { h.vs.erase(this); }
  virtual void mark() = 0;  // mark everything in this view
  virtual void move() = 0;  // move everything in this view
};


struct π0hds : virtual π0hv,  // data-stack heap view
               virtual π0sv
{
  V<π0r> s;

  π0hds()             = delete;
  π0hds(π0hds const&) = delete;
  π0hds(π0hds&&)      = delete;
  π0hds(π0h &h_) : π0hv(h_) {}

  void mark() { for (let   x : s)     π0hv::h.mark(x); }
  void move() { for (auto &x : s) x = π0hv::h.move(x); }

  π0sv *up() const { return nullptr; }

  void                operator<<(π0r x)      { s.push_back(x); }
  template<O9 T> void operator<<(T const &x) { *this << (π0hv::h << x); }

  π0r operator[](uN i) const { return s.at(s.size() - i - 1); }
  void      drop(uN n = 1)   { s.resize(s.size() - n); }
  uN        size()     const { return s.size(); }
};


struct π0hss : virtual π0hv,  // split-stack heap view
               virtual π0sv
{
  π0sv const &d;              // base data stack
  uN          n;              // number of elements "deleted"
  V<π0r>      s;              // elements we've added

  π0hss()             = delete;
  π0hss(π0hss const&) = delete;
  π0hss(π0hss&&)      = delete;
  π0hss(π0h &h_, π0sv &d_) : π0hv(h_), d(d_) {}

  // NOTE: no need to mark/move d, since it's an independent
  // member of h.vs
  void mark() { for (let   x : s)     π0hv::h.mark(x); }
  void move() { for (auto &x : s) x = π0hv::h.move(x); }

  π0sv *up() const { return Cc<π0sv*>(&d); }

  void                operator<<(π0r x)      { s.push_back(x); }
  template<O9 T> void operator<<(T const &x) { *this << (π0hv::h << x); }

  π0r operator[](uN i) const
  { return i < s.size() ? s.at(s.size() - i - 1) : d[i - s.size() + n]; }

  void drop(uN x = 1)
  { let l = std::min(x, s.size());
    x -= l; s.resize(s.size() - l);
    n += l; }

  uN size() const { return d.size() - n + s.size(); }
};


struct π0hdf : virtual π0hv  // stack-of-frames heap view
{
  V<π0r> d;  // all frames concatenated together
  V<uN>  n;  // first variable within each frame

  π0hdf()             = delete;
  π0hdf(π0hdf const&) = delete;
  π0hdf(π0hdf&&)      = delete;
  π0hdf(π0h &h_) : π0hv(h_) {}

  void mark() { for (let   x : d)     π0hv::h.mark(x); }
  void move() { for (auto &x : d) x = π0hv::h.move(x); }

  π0r &operator[](uN i) { return d[n.back() + i]; }

  π0hdf &push(uN s) { n.push_back(d.size()); d.resize(d.size() + s, nullptr); return *this; }
  π0hdf  &pop()     { d.resize(n.back()); n.pop_back(); return *this; }
};


struct π0hnf : virtual π0hv  // native frame heap view
{
  V<i9>        v;  // TODO: store pointers instead
  V<F<void()>> f;  // TODO: add array of non-tracked things here

  π0hnf()             = delete;
  π0hnf(π0hnf const&) = delete;
  π0hnf(π0hnf&&)      = delete;
  π0hnf(π0h &h_, uN vs = 4) : π0hv(h_) { v.reserve(vs); }

  void mark() { for (let x : v) π0hv::h.mark(x); }
  void move()
    { for (auto &x : v) x = π0hv::h.move(x);
      for (auto &x : f) x(); }

  i9 &operator<<(π0r x) {
    TODO("FIXME: π0hnf can't return refs within memory-moving containers");
    v.push_back(x); return v.back(); }
};


}

#include "end.hh"


#endif
