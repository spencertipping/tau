#ifndef τφ_h
#define τφ_h


#include "types.hh"
#include "begin.hh"

namespace τ
{


template<class T> struct φ;
template<class T> struct φr;
struct φc;


// φ parser ∷ φc → φr<T>
template<class T>
struct φ
{
  virtual ~φ() {};

  virtual St          name()          const = 0;
  virtual φr<T> operator()(φc const&) const = 0;
};


// Parse result, which is either:
//   accept ∷ (x, i) → (y, j)
//   fail   ∷ (x, i) → (e, t, j)
template<class T>
struct φr
{
  Sp<Stc>     x;
  uNc         i;
  uNc         j;
  Op<T> const y;
  Stc         e;
  Stc         t;

  bool is_a() const { return  y; }
  bool is_f() const { return !y; }

  bool operator>(φr<T> const &o) const { return j > o.j; }
};


// φ input context: input string and current φ trace
struct φc
{
  // Create a new root context
  φc(St x_) : x(new St{x_}), i(0), n(""), p(nullptr) {}
  φc(Sp<Stc> x_, uN i_, Stc n_, φc const *p_)
    : x(x_), i(i_), n(n_), p(p_) {}

                    φc at(uN j)          const { return φc{x, j, n, p}; }
  template<class T> φc at(φ<T> const &f) const { return φc{x, i, f.name(), this}; }

  template<class T> φr<T> a (T  y, uN j) const { return φr<T>{x, i, j, y, {}, {}}; }
  template<class T> φr<T> f (St e, uN j) const { return φr<T>{x, i, j, {}, e, trace()}; }

  St trace() const { St r; trace_(r); return r; }


protected:
  Sp<Stc> const  x;  // input
  uNc            i;  // input offset
  Stc            n;  // current parser name
  φc      const *p;  // parent context, or nullptr

  void trace_(St &into) const
    { if (p) p->trace_(into), into.append(" ");
      into.append(n); }
};


template<class T>
O &operator<<(O &s, φ<T> const &f)
{
  return s << f.name();
}


}

#include "end.hh"


#endif
