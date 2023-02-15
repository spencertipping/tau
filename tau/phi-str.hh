#ifndef τφstr_h
#define τφstr_h


#include <algorithm>


#include "types.hh"
#include "strings.hh"
#include "phi.hh"

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
  φd_ &def(Stc &k, φ<T> p, Xs const&... xs) { ps[k] = p; return def(xs...); }
  φd_ &def()                                {            return *this; }


  St name() const
    { St r = "{\n";
      for (let &[k, p] : ps) r += "\"" + k + "\" → " + p->name() + "\n";
      return r + "}"; }

  φr_<T> operator()(φc_ const &x) const
    { V<St> ks;
      for (let &[k, p] : ps)
        if (k.size() <= x.l() && x.sub(k.size()) == k) ks.push_back(k);
      std::sort(ks.begin(), ks.end(),
                [](Stc &a, Stc &b) { return a.size() > b.size(); });
      // TODO: collect failures
      for (let &k : ks)
      { let s = (*ps.at(k))(x.at(x.i() + k.size()));
        if (s.is_a()) return s; }
      return x.at(*this).template f<T>("no matching prefix, or all alternatives failed", x.i()); }


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
           : x.f<T>("literal mismatch", x.i()); }

  St l;
  T  y;
};


// Charset: any-of, formed into a string (NOTE: 7-bit ASCII)
struct φcs_ : public virtual φ_<St>
{
  φcs_(chc *cs_, bool n_ = false, uN min_ = 0, uN max_ = -1)
    : cs(cs_), n(n_), min(min_), max(max_) {}

  St name() const { return (Ss{} << cs).str(); }
  φr_<St> operator()(φc_ const &x) const
    { St r;
      uN i = 0;
      while (i < max && i < x.l() && cs[x[i]] ^ n) r += x[i++];
      if (i < min) return x.at(*this).f<St>("too few chars", x.i() + i);
      return x.a(r, x.i() + r.size()); }

  cs7  cs;
  bool n;    // if true, negate
  uN   min;  // lower limit on number of chars to match
  uN   max;  // upper limit on number of chars to match
};


// End of input detector
struct φE_ : public virtual φ_<bool>
{
  φE_() {}
  St name() const { return "E"; }
  φr_<bool> operator()(φc_ const &x) const
    { return x.l()
           ? x.f<bool>((Ss{} << "not eof: " << x.i() << " ≠ " << x.n()).str(), x.i())
           : x.a(true, x.i()); }
};


}

#include "end.hh"


#endif
