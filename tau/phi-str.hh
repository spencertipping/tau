#ifndef τφstr_h
#define τφstr_h


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
  φd_(Xs... xs) { def(xs...); }

  template<class... Xs>
  φd_ &def(Stc &k, φ<T> p, Xs... xs) { ps[k] = p; return def(xs...); }
  φd_ &def()                         {            return *this; }

  St name() const;
  φr_<T> operator()(φc_ const&) const;

  M<St, φ<T>> ps;
};


// Literal string, with literal result
template<class T>
struct φl_ : public virtual φ_<T>
{
  φl_(St l_, T y_) : l(l_), y(y_) {}

  St name() const { return "\"" + l + "\""; }
  φr_<T> operator()(φc_ const&) const;

  St l;
  T  y;
};


// Charset: any-of, formed into a string
struct φcs_ : public virtual φ_<St>
{
  φcs_(chc *cs_) : cs(cs_) {}

  St name() const { return (Ss{} << cs).str(); }
  φr_<St> operator()(φc_ const&) const;

  cs7 cs;
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
