#ifndef τφ_h
#define τφ_h


#include "types.hh"
#include "begin.hh"

namespace τ
{


template<class T> struct φ;
template<class T> struct φr;


// Parse result, which is either:
//   accept (x, i) → (y, j)
//   fail   (x, i) → (e, t, j)
template<class T>
struct φr
{
  Stc        &x;
  uNc         i;
  uNc         j;
  Op<T> const y;
  Stc         e;
  Stc         t;

  bool is_a() const { return  y; }
  bool is_f() const { return !y; }
};


// φ input context: input string and current φ trace
struct φc
{
  // Create a new root context
  φc(St x_) : x(new St{x_}), i(0), n("/"), p(nullptr) {}

  template<class T> φc    at(φ<T>, uN) const;
  template<class T> φr<T> a (T,    uN) const;
  template<class T> φr<T> f (St,   uN) const;


protected:
  Sp<Stc> const  x;  // input
  uNc            i;  // input offset
  Stc            n;  // current parser name
  φc      const *p;  // parent context, or nullptr
};


template<class T>
struct φ
{
  virtual ~φ() {};

  virtual St          name()          const = 0;
  virtual φr<T> operator()(φc const&) const = 0;
};


template<class T>          Sp<φ<T>>       operator/(Sp<φ<T>>, Sp<φ<T>>);
template<class T, class U> Sp<φ<P<T, U>>> operator*(Sp<φ<T>>, Sp<φ<U>>);


template<class T>
O &operator<<(O &s, φ<T> const &f)
{
  return s << f.name();
}


}

#include "end.hh"


#endif
