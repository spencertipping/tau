#ifndef τπ0_gc_heapview_h
#define τπ0_gc_heapview_h


#include "debug.hh"
#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"


// Tests the GC to make sure the algorithm is invariant wrt external
// root ordering.
#if !defined(τπ0debug_heapview_shuffle)
# define τπ0debug_heapview_shuffle 0
#endif

#if τπ0debug_heapview_shuffle
# include <algorithm>
# include <random>
# warning π₀ debug heapview shuffle enabled
#endif


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


struct π0hgs : public virtual π0hv  // global set
{
  M<u9_symbol, π0r> s;

  π0hgs()             = delete;
  π0hgs(π0hgs const&) = delete;
  π0hgs(π0hgs &&x) : π0hv(x.h), s(std::move(x.s)) {}
  π0hgs(π0h   &h_) : π0hv(h_) {}
  π0hgs(π0h   &h_, M<u9_symbol, π0r> const &s_) : π0hv(h_), s(s_) {}

  void mark() { for (let  &[_, v] : s)     h.mark(v); }
  void move() { for (auto &[_, v] : s) v = h.move(v); }

  π0r &operator[](u9_symbol const &k) { return s[k]; }
  π0hgs        &x(u9_symbol const &k) { s.erase(k); return *this; }
  bool          i(u9_symbol const &k) { return s.contains(k); }
};


struct π0hds : public virtual π0hv,  // data-stack heap view
               public virtual π0sv
{
  V<π0r> s;

  π0hds()             = delete;
  π0hds(π0hds const&) = delete;
  π0hds(π0hds&&)      = delete;
  π0hds(π0h &h_) : π0hv(h_) {}

  void mark() { for (let   x : s)     h.mark(x); }
  void move() { for (auto &x : s) x = h.move(x); }

  π0sv *up() const { return nullptr; }

  void                operator<<(π0r x)      { s.push_back(x); }
  template<O9 T> void operator<<(T const &x) { *this << (h << x); }

  i9 operator[](uN i) const { return s.at(s.size() - i - 1); }
  void     drop(uN n = 1)   { s.resize(s.size() - n); }
  uN       size()     const { return s.size(); }
};


struct π0hss : public virtual π0hv,  // split-stack heap view
               public virtual π0sv
{
  π0sv const &d;  // base data stack
  uN          n;  // number of elements "deleted"
  V<π0r>      s;  // elements we've added

  π0hss()             = delete;
  π0hss(π0hss const&) = delete;
  π0hss(π0hss&&)      = delete;
  π0hss(π0h &h_, π0sv &d_, uN n_ = 0) : π0hv(h_), d(d_), n(n_) {}

  // NOTE: no need to mark/move d, since it's an independent
  // member of h.vs
  void mark() { for (let   x : s)     h.mark(x); }
  void move() { for (auto &x : s) x = h.move(x); }

  π0sv *up() const { return Cc<π0sv*>(&d); }

  void                operator<<(π0r x)      { s.push_back(x); }
  template<O9 T> void operator<<(T const &x) { *this << (h << x); }

  i9 operator[](uN i) const
    { return i < s.size() ? i9{s.at(s.size() - i - 1)} : d[i - s.size() + n]; }

  void drop(uN x = 1)
    { let l = std::min(x, Sc<uN>(s.size()));
      x -= l; s.resize(s.size() - l);
      n += l; }

  uN size() const { return d.size() - n + s.size(); }
};


struct π0hdf : public virtual π0hv  // stack-of-frames heap view
{
  V<π0r> d;  // all frames concatenated together
  V<uN>  n;  // first variable within each frame

  π0hdf()             = delete;
  π0hdf(π0hdf const&) = delete;
  π0hdf(π0hdf&&)      = delete;
  π0hdf(π0h &h_) : π0hv(h_) {}

  void mark() { for (let   x : d)     h.mark(x); }
  void move() { for (auto &x : d) x = h.move(x); }

  π0r &operator[](uN i) { return d[n.back() + i]; }

  π0hdf &push(uN s) { n.push_back(d.size()); d.resize(d.size() + s, nullptr); return *this; }
  π0hdf  &pop()     { d.resize(n.back()); n.pop_back(); return *this; }
};


struct π0hnf : public virtual π0hv  // native frame heap view
{
  V<i9*>        v;
  V<V<i9>*>     s;
  V<M<i9, i9>*> m;
  V<F<void()>>  f;

  π0hnf()             = delete;
  π0hnf(π0hnf const&) = delete;
  π0hnf(π0hnf&&)      = delete;
  π0hnf(π0h &h_, uN vs = 16) : π0hv(h_) { v.reserve(vs); }

#if τπ0debug_heapview_shuffle
  void mark()
    { std::shuffle(v.begin(), v.end(), std::default_random_engine(now().time_since_epoch().count()));
      std::shuffle(s.begin(), s.end(), std::default_random_engine(now().time_since_epoch().count()));
      for (let x : v) h.mark(x->a);
      for (let x : s)
      { std::shuffle(x->begin(), x->end(), std::default_random_engine(now().time_since_epoch().count()));
        for (let y : *x) h.mark(y); } }
#else
  void mark()
    { for (let x : v)                        h.mark(x->a);
      for (let x : m) for (let &[k, v] : *x) h.mark(k), h.mark(v);
      for (let x : s) for (let y : *x)       h.mark(y); }
#endif

  void move()
    { for (let   x : v)                         x->a = h.move(x->a);
      for (let   x : s) for (auto &y : *x)      y    = h.move(y);
      for (let   x : m) for (auto &[k, v] : *x) Cc<i9&>(k) = h.move(k), v = h.move(v);
      for (auto &x : f)                         x(); }

  template<class... Xs> π0hnf &operator()(i9 *x,         Xs... xs) { v.push_back(x);                        return (*this)(xs...); }
  template<class... Xs> π0hnf &operator()(V<i9> *x,      Xs... xs) { s.push_back(x);                        return (*this)(xs...); }
  template<class... Xs> π0hnf &operator()(M<i9, i9> *x,  Xs... xs) { m.push_back(x);                        return (*this)(xs...); }
  template<class... Xs> π0hnf &operator()(F<void()> &&x, Xs... xs) { f.push_back(std::move(x)); f.back()(); return (*this)(xs...); }
  π0hnf &operator()() { return *this; }
};


struct π0hgl : public virtual π0hv  // GC lock (asserts that no GC will happen)
{
  π0hgl(π0h &h_) : π0hv(h_) {}
  void mark() { A(0, "GC during lock"); }
  void move() {}
};


struct π0hη : public virtual π0hv   // i9-keyed map
{
  M<i9, π0r> s;

  π0hη()            = delete;
  π0hη(π0hη const&) = delete;
  π0hη(π0hη &&x) : π0hv(x.h), s(std::move(x.s)) {}
  π0hη(π0h  &h_) : π0hv(h_) {}

  void mark() { for (let &[k, v] : s) h.mark(k.a), h.mark(v); }
  void move()
    { for (auto &[k, v] : s)
      { // NOTE: modification is safe here because hashing is GC-invariant
        Cc<i9&>(k).a = h.move(k.a);
        v            = h.move(v); } }

  π0r &operator[](i9 const &k) { return s[k]; }
  π0hη         &x(i9 const &k) { s.erase(k); return *this; }
  bool          i(i9 const &k) { return s.contains(k); }
};


}

#include "end.hh"


#endif
