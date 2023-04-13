#ifndef τφstr_h
#define τφstr_h


#include <algorithm>
#include <regex>


#include "types.hh"
#include "strings.hh"
#include "phi.hh"
#include "phi-fn.hh"

#include "begin.hh"

namespace τ
{


// Longest-prefix dispatch
template<class T>
struct φd_ : public virtual φ_<T>
{
  template<class... Xs>
  φd_(Xs const&... xs) { def(xs...); }

  template<class... Xs>
  φd_ &def(Stc &k, φ<T> p, Xs const&... xs)
    { A(!ps.contains(k), *this << ": redefining " << k);
      ps[k] = p;
      return def(xs...); }

  template<class... Xs>
  φd_ &def(Stc &k, T x, Xs const&... xs)
    { return def(k, φ<T>{new φR_<T>(x)}, xs...); }

  φd_ &def() { return *this; }


  St name() const
    { St r = "{\n";
      for (let &[k, p] : ps) r += "\"" + k + "\" → " + p.name() + "\n";
      return r + "}"; }

  φr_<T> operator()(φc_ const &x) const
    { V<Stc*> ks;
      for (let &[k, p] : ps)
        if (x.sw(k)) ks.push_back(&k);
      std::sort(ks.begin(), ks.end(),
                [](Stc *a, Stc *b) { return a->size() > b->size(); });
      for (let &k : ks)
      { let s = (*ps.at(*k))(x.at(x.i() + k->size()));
        if (s.is_a()) return s; }
      return x.template f<T>(this, x.i()); }


  M<St, φ<T>> ps;
};


// Literal string, with literal result
template<class T>
struct φl_ : public virtual φ_<T>
{
  φl_(St l_, T y_) : l(l_), y(y_) {}

  St name() const { return "\"" + l + "\""; }
  φr_<T> operator()(φc_ const &x) const
    { return x.l() >= l.size() && x.sub(l.size()) == l
           ? x.a(y, x.i() + l.size())
           : x.f<T>(this, x.i()); }

  St l;
  T  y;
};


// Charset: any-of, formed into a string (NOTE: 7-bit ASCII)
struct φcs_ : public virtual φ_<St>
{
  φcs_(chc *cs_, bool n_ = false, uN min_ = 0, uN max_ = -1)
    : cs(cs_), n(n_), min(min_), max(max_) {}

  St name() const { return (Ss{} << cs).str(); }
  φr_<St> operator()(φc_ const&) const;

  cs7  cs;
  bool n;    // if true, negate
  uN   min;  // lower limit on number of chars to match
  uN   max;  // upper limit on number of chars to match
};


// UTF-8 encoded character predicate (note: max/min specified as #chars,
// not #bytes)
struct φucs_ : public virtual φ_<St>
{
  φucs_(F<bool(u64)> f_, uN min_ = 0, uN max_ = -1)
    : f(f_), min(min_), max(max_) {}

  St name() const { return "φucs"; }
  φr_<St> operator()(φc_ const&) const;

  F<bool(u64)> f;
  uN           min;
  uN           max;
};


// Regex match, beginning at a minimum number of bytes
struct φre_ : public virtual φ_<V<St>>
{
  φre_(St src_) : src(src_), r("^(?:" + src + ")") {}

  St name() const { return "/" + src + "/"; }
  φr_<V<St>> operator()(φc_ const&) const;

  St src;
  Re r;
};


// Quote the string that was parsed by something
template<class T>
struct φq_ : public virtual φ_<St>
{
  φq_(φ<T> p_) : p(p_) {}

  St name() const { return "'" + p.name(); }
  φr_<St> operator()(φc_ const &x) const
    { let s = p(x);
      return s.is_a()
           ? x.template a<St>(x.sub(s.j - x.i()), s.j)
           : s.template cast<St>(); }

  φ<T> p;
};


// End of input detector (wraps another parser)
template<class T>
struct φE_ : public virtual φ_<T>
{
  φE_(φ<T> p_) : p(p_) {}

  St name() const { return "E" + p.name(); }
  φr_<T> operator()(φc_ const &x) const
    { let s = p(x);
      if (s.is_f())     return s;
      if (s.j != x.n()) return x.f<T>(this, s.j);
      return s; }

  φ<T> p;
};


}

#include "end.hh"


#endif
