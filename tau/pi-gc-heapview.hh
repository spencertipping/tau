#ifndef τπ_gc_heapview_h
#define τπ_gc_heapview_h


#include "debug.hh"
#include "types.hh"
#include "utf9.hh"
#include "pi-types.hh"
#include "pi-gc-heap.hh"


// Tests the GC to make sure the algorithm is invariant wrt external
// root ordering.
#if !defined(τπdebug_heapview_shuffle)
# define τπdebug_heapview_shuffle 0
#endif

#if τπdebug_heapview_shuffle
# include <algorithm>
# include <random>
# warning π₀ debug heapview shuffle enabled
#endif


#include "begin.hh"

namespace τ
{


struct πhv  // heap view: a part of the root set
{
  πh &h;
  πhv(πh &h_) : h(h_) { h.vs.insert(this); }
  virtual ~πhv()      { h.vs.erase(this); }
  virtual void mark() = 0;  // mark everything in this view
  virtual void move() = 0;  // move everything in this view
};


struct πhgs : public virtual πhv  // global set
{
  M<u9_symbol, πr> s;

  πhgs()            = delete;
  πhgs(πhgs const&) = delete;
  πhgs(πhgs &&x) : πhv(x.h), s(std::move(x.s)) {}
  πhgs(πh   &h_) : πhv(h_) {}
  πhgs(πh   &h_, M<u9_symbol, πr> const &s_) : πhv(h_), s(s_) {}

  void mark() { for (let  &[_, v] : s)     h.mark(v); }
  void move() { for (auto &[_, v] : s) v = h.move(v); }

  πr &operator[](u9_symbol const &k) { return s[k]; }
  πhgs        &x(u9_symbol const &k) { s.erase(k); return *this; }
  bool         i(u9_symbol const &k) { return s.contains(k); }
};


struct πhds : public virtual πhv,  // data-stack heap view
              public virtual πsv
{
  V<πr> s;

  πhds()            = delete;
  πhds(πhds const&) = delete;
  πhds(πhds&&)      = delete;
  πhds(πh &h_) : πhv(h_) {}

  void mark() { for (let   x : s)     h.mark(x); }
  void move() { for (auto &x : s) x = h.move(x); }

  πsv *up() const { return nullptr; }

  void                operator<<(πr x)      { s.push_back(x); }
  template<O9 T> void operator<<(T const &x) { *this << (h << x); }

  i9 operator[](uN i) const { return s.at(s.size() - i - 1); }
  void     drop(uN n = 1)   { s.resize(s.size() - n); }
  uN       size()     const { return s.size(); }
};


struct πhss : public virtual πhv,  // split-stack heap view
              public virtual πsv
{
  πsv const &d;  // base data stack
  uN         n;  // number of elements "deleted"
  V<πr>      s;  // elements we've added

  πhss()            = delete;
  πhss(πhss const&) = delete;
  πhss(πhss&&)      = delete;
  πhss(πh &h_, πsv &d_, uN n_ = 0) : πhv(h_), d(d_), n(n_) {}

  // NOTE: no need to mark/move d, since it's an independent
  // member of h.vs
  void mark() { for (let   x : s)     h.mark(x); }
  void move() { for (auto &x : s) x = h.move(x); }

  πsv *up() const { return Cc<πsv*>(&d); }

  void                operator<<(πr x)      { s.push_back(x); }
  template<O9 T> void operator<<(T const &x) { *this << (h << x); }

  i9 operator[](uN i) const
    { return i < s.size() ? i9{s.at(s.size() - i - 1)} : d[i - s.size() + n]; }

  void drop(uN x = 1)
    { let l = std::min(x, Sc<uN>(s.size()));
      x -= l; s.resize(s.size() - l);
      n += l; }

  uN size() const { return d.size() - n + s.size(); }
};


struct πhdf : public virtual πhv  // stack-of-frames heap view
{
  V<πr> d;  // all frames concatenated together
  V<uN> n;  // first variable within each frame

  πhdf()             = delete;
  πhdf(πhdf const&) = delete;
  πhdf(πhdf&&)      = delete;
  πhdf(πh &h_) : πhv(h_) {}

  void mark() { for (let   x : d)     h.mark(x); }
  void move() { for (auto &x : d) x = h.move(x); }

  πr &operator[](uN i) { return d[n.back() + i]; }

  πhdf &push(uN s) { n.push_back(d.size()); d.resize(d.size() + s, nullptr); return *this; }
  πhdf  &pop()     { d.resize(n.back()); n.pop_back(); return *this; }
};


struct πhnf : public virtual πhv  // native frame heap view
{
  V<i9*>        v;
  V<V<i9>*>     s;
  V<M<i9, i9>*> m;
  V<F<void()>>  f;

  πhnf()             = delete;
  πhnf(πhnf const&) = delete;
  πhnf(πhnf&&)      = delete;
  πhnf(πh &h_, uN vs = 16) : πhv(h_) { v.reserve(vs); }

#if τπdebug_heapview_shuffle
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

  template<class... Xs> πhnf &operator()(i9 *x,         Xs... xs) { v.push_back(x);                        return (*this)(xs...); }
  template<class... Xs> πhnf &operator()(V<i9> *x,      Xs... xs) { s.push_back(x);                        return (*this)(xs...); }
  template<class... Xs> πhnf &operator()(M<i9, i9> *x,  Xs... xs) { m.push_back(x);                        return (*this)(xs...); }
  template<class... Xs> πhnf &operator()(F<void()> &&x, Xs... xs) { f.push_back(std::move(x)); f.back()(); return (*this)(xs...); }
  πhnf &operator()() { return *this; }
};


struct πhgl : public virtual πhv  // GC lock (asserts that no GC will happen)
{
  πhgl(πh &h_) : πhv(h_) {}
  void mark() { A(0, "GC during lock"); }
  void move() {}
};


struct πhη : public virtual πhv   // i9-keyed map
{
  M<i9, πr> s;

  πhη()            = delete;
  πhη(πhη const&) = delete;
  πhη(πhη &&x) : πhv(x.h), s(std::move(x.s)) {}
  πhη(πh  &h_) : πhv(h_) {}

  void mark() { for (let &[k, v] : s) h.mark(k.a), h.mark(v); }
  void move()
    { for (auto &[k, v] : s)
      { // NOTE: modification is safe here because hashing is GC-invariant
        Cc<i9&>(k).a = h.move(k.a);
        v            = h.move(v); } }

  πr &operator[](i9 const &k) { return s[k]; }
  πhη         &x(i9 const &k) { s.erase(k); return *this; }
  bool         i(i9 const &k) { return s.contains(k); }
};


}

#include "end.hh"


#endif
