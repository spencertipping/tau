#ifndef τπ0_gc_heapview_h
#define τπ0_gc_heapview_h


#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"

#include "pi0-begin.hh"

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


π0TGs π0sv  // stack view
{
  π0TS;
  virtual ~π0sv() {}
  virtual π0T(π0sv)    *up()    const = 0;
  virtual uN         size()     const = 0;
  virtual π0R  operator[](uN i) const = 0;
  virtual void operator<<(π0R x)      = 0;
  virtual void       drop(uN n = 1)   = 0;

  π0R pop() { let r = (*this)[0]; drop(1); return r; }
};


π0TGs π0hds : virtual π0T(π0hv),  // data-stack heap view
              virtual π0T(π0sv)
{
  π0TS;
  V<π0R> s;

  π0hds()                  = delete;
  π0hds(π0T(π0hds) const&) = delete;
  π0hds(π0T(π0hds)&&)      = delete;
  π0hds(π0T(π0h) &h_) : π0T(π0hv)(h_) {}

  void mark() { for (let   x : s)     π0T(π0hv)::h.mark(x); }
  void move() { for (auto &x : s) x = π0T(π0hv)::h.move(x); }

  π0T(π0sv) *up() const { return nullptr; }

  void      operator<<(π0R x)         { s.push_back(x); }
  π0TO void operator<<(π0To const &x) { *this << (π0T(π0hv)::h << x); }

  π0R operator[](uN i) const { return s.at(s.size() - i - 1); }
  void      drop(uN n = 1)   { s.resize(s.size() - n); }
  uN        size()     const { return s.size(); }
};


π0TGs π0hss : virtual π0T(π0hv),  // split-stack heap view
              virtual π0T(π0sv)
{
  π0TS;
  π0T(π0sv) const &d;             // base data stack
  uN               n;             // number of elements "deleted"
  V<π0R>           s;             // elements we've added

  π0hss()                  = delete;
  π0hss(π0T(π0hss) const&) = delete;
  π0hss(π0T(π0hss)&&)      = delete;
  π0hss(π0T(π0h) &h_, π0T(π0sv) &d_) : π0T(π0hv)(h_), d(d_) {}

  // NOTE: no need to mark/move d, since it's an independent
  // member of h.vs
  void mark() { for (let   x : s)     π0T(π0hv)::h.mark(x); }
  void move() { for (auto &x : s) x = π0T(π0hv)::h.move(x); }

  π0T(π0sv) *up() const { return &d; }

  void      operator<<(π0R x)         { s.push_back(x); }
  π0TO void operator<<(π0To const &x) { *this << (π0T(π0hv)::h << x); }

  π0R operator[](uN i) const
  { return i < s.size() ? s.at(s.size() - i - 1) : d[i - s.size() + n]; }

  void drop(uN x = 1)
  { let l = std::min(x, s.size());
    x -= l; s.resize(s.size() - l);
    n += l; }

  uN size() const { return d.size() - n + s.size(); }
};


π0TGs π0hdf : virtual π0T(π0hv)  // stack-of-frames heap view
{
  π0TS;
  V<π0R> d;  // all frames concatenated together
  V<uN>  n;  // first variable within each frame

  π0hdf()                  = delete;
  π0hdf(π0T(π0hdf) const&) = delete;
  π0hdf(π0T(π0hdf)&&)      = delete;
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

  π0hnf()                  = delete;
  π0hnf(π0T(π0hnf) const&) = delete;
  π0hnf(π0T(π0hnf)&&)      = delete;
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

#include "pi0-end.hh"


#endif
