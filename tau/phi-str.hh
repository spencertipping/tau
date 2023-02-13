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
      for (let &k : ks) { let s = (*ps.at(k))(x); if (s.is_a()) return s; }
      return x.at(*this).template f<T>("no matching prefix", x.i()); }


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
  φcs_(chc *cs_, bool n_ = false, uN limit_ = -1)
    : cs(cs_), n(n_), l(limit_) {}

  St name() const { return (Ss{} << cs).str(); }
  φr_<St> operator()(φc_ const &x) const
    { St r;
      uN i = x.i(), j = 0;
      while (j < l && i + j < x.l() && cs[x[i]] ^ n) r += x[i + j];
      return x.a(r, x.i() + r.size()); }

  cs7  cs;
  bool n;  // if true, negate
  uN   l;  // limit on number of chars to match
};


// End of input detector
struct φE_ : public virtual φ_<bool>
{
  φE_() {}
  St name() const { return "E"; }
  φr_<bool> operator()(φc_ const &x) const
    { return x.i() == x.l()
           ? x.a(true, x.i())
           : x.f<bool>("not eof", x.i()); }
};


}

#include "end.hh"


#endif
