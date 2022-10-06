#ifndef τπ0int_h
#define τπ0int_h


#include "types.hh"
#include "numerics.hh"
#include "Phi.hh"
#include "Lambda.hh"
#include "zeta.hh"
#include "utf9.hh"

#include "pi0-types.hh"
#include "pi0-gc.hh"


#include "begin.hh"

namespace τ
{


struct π0int
{
  B            q;  // quoted statics
  V<π0f> const f;  // predefined functions
  V<uN>  const c;  // bytecode program (indexes into f[])
  V<uN>        r;  // return stack
  π0h          h;  // data stack + local frames

  π0int(B &&q_, V<π0f> &&f_, V<uN> &&c_, uN c0)
    : q(std::move(q_)), f(std::move(f_)), c(std::move(c_))
    { r.push_back(c0); }

  i9 operator[](uN i)   {                          return i9{q.data() + i}; }
  operator bool() const {                          return !r.empty(); }
  π0int     &go()       { while (*this) (*this)(); return *this; }

  template<o9mapped T>
  π0int &operator<<(T const &x) { h.dpush(x); return *this; }

  π0int &dup() { h.d.push_back(h.d.back());          return *this; }
  π0int &dip() { h.d.push_back(h.d[h.d.size() - 2]); return *this; }
  i9     pop() { let i = h.di(0); h.dpop();          return i; }

  π0int &run(uN l)
    { let n = r.size();
      r.push_back(l);
      while (r.size() > n) (*this)();
      return *this; }

#if τπ0debug_bounds_checks
  π0int &operator()()
    { A(!r.empty(), "π₀i() r=∅");
      f.at(c.at(r.back()++))(*this); return *this; }
#else
  π0int &operator()() { f[c[r.back()++]](*this); return *this; }
#endif
};


#if τdebug_iostream
O &operator<<(O &s, π0int const &i)
{
  s << "π₀i qs=" << i.q.size()
    << " fs=" << i.f.size()
    << " cs=" << i.c.size()
    << " rs=" << i.r.size()
    << " r=";
  if (!i.r.empty())
    for (iN j = i.r.size() - 1; j >= 0; --j)
      s << i.r[j] << " ";
  return s << i.h << std::endl;
}
#endif


}

#include "end.hh"


#endif
