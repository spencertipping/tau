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
struct φd : public virtual φ<T>
{
  template<class... Xs>
  φd(Xs... xs) { def(xs...); }

  template<class... Xs>
  φd &def(Stc &k, Sp<φ<T>> p, Xs... xs) { ps[k] = p; return def(xs...); }
  φd &def()                             {            return *this; }

  St name() const;
  φr<T> operator()(φc const&) const;

  M<St, Sp<φ<T>>> ps;
};


// Literal string, with literal result
template<class T>
struct φl : public virtual φ<T>
{
  φl(St l_, T y_) : l(l_), y(y_) {}

  St name() const { return "\"" + l + "\""; }
  φr<T> operator()(φc const&) const;

  St l;
  T  y;
};


// Charset: any-of, formed into a string
struct φcs : public virtual φ<St>
{
  φcs(chc *cs_) : cs(cs_) {}

  St name() const { return (Ss{} << cs).str(); }
  φr<St> operator()(φc const&) const;

  cs7 cs;
};


// End of input detector
struct φE : public virtual φ<bool>
{
  φE() {}
  St name() const { return "E"; }
  φr<bool> operator()(φc const &x) const
    { return x.i() == x.l()
           ? x.a(true, x.i())
           : x.f<bool>("not eof", x.i()); }
};


}

#include "end.hh"


#endif
