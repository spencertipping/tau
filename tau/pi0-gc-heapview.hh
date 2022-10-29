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

  i9 operator[](uN i) const { return s.at(s.size() - i - 1); }
  void     drop(uN n = 1)   { s.resize(s.size() - n); }
  uN       size()     const { return s.size(); }
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

  i9 operator[](uN i) const
    { return i < s.size() ? i9{s.at(s.size() - i - 1)} : d[i - s.size() + n]; }

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
  V<i9*>       v;
  V<V<i9>*>    s;
  V<F<void()>> f;

  π0hnf()             = delete;
  π0hnf(π0hnf const&) = delete;
  π0hnf(π0hnf&&)      = delete;
  π0hnf(π0h &h_, uN vs = 16) : π0hv(h_) { v.reserve(vs); }

  void mark()
    { for (let x : v)                  π0hv::h.mark(x->a);
      for (let x : s) for (let y : *x) π0hv::h.mark(y); }

  void move()
    { for (let   x : v)                 x->a = π0hv::h.move(x->a);
      for (let   x : s) for (auto &y : *x) y = π0hv::h.move(y);
      for (auto &x : f)                    x(); }

  template<class... Xs> π0hnf &operator()(i9 *x,         Xs... xs) { v.push_back(x);                        return (*this)(xs...); }
  template<class... Xs> π0hnf &operator()(V<i9> *x,      Xs... xs) { s.push_back(x);                        return (*this)(xs...); }
  template<class... Xs> π0hnf &operator()(F<void()> &&x, Xs... xs) { f.push_back(std::move(x)); f.back()(); return (*this)(xs...); }
  π0hnf &operator()() { return *this; }
};


struct π0hgl : virtual π0hv  // GC lock (asserts that no GC will happen)
{
  π0hgl(π0h &h_) : π0hv(h_) {}
  void mark() { A(0, "GC during lock"); }
  void move() {}
};


}

#include "end.hh"


#endif
