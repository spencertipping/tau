#ifndef τπ0int_h
#define τπ0int_h


#include "types.hh"
#include "numerics.hh"
#include "Lambda.hh"
#include "zeta.hh"
#include "utf9.hh"

#include "pi0-types.hh"
#include "pi0-gc.hh"


#include "pi0-begin.hh"

namespace τ
{


π0TGs π0int
{
  π0TS;
  π0T(π0h)         &h;   // data stack + local frames
  π0T(π0abi) const &a;   // ABI (native functions)
  π0T(π0p)   const  p;   // bytecode program
  V<uN>             r;   // return stack
  π0T(π0hdf)        f;   // frame stack
  π0T(π0hds)        d;   // base data stack
  π0T(π0sv)        *dv;  // current data stack view


  π0int(π0T(π0abi) &a_, π0T(π0p) &&p_, uN c0)
  : a(a_), p(std::move(p_)), f(h), d(h), dv(&d)
  { A(p.v == a.v, "π₀ ABI mismatch: " << p.v << " ≠ " << a.v);
    r.push_back(c0); }

  operator bool() const {                          return !r.empty(); }
  π0int     &go()       { while (*this) (*this)(); return *this; }
  π0int    &run(uN l)
    { let n = r.size();
      r.push_back(l);
      while (r.size() > n) (*this)();
      return *this; }

  // FIXME: standardize this API, maybe π0int even follows the π0sv API
  i9 operator[](uN i) { return i9{p.q.data() + i}; }


  π0int &spush() { dv = new π0T(π0sv){h, *dv};          return *this; }
  π0int &spop()  { let v = dv; dv = dv->up(); delete v; return *this; }


#if τπ0debug_bounds_checks
  π0int &operator()()
  { A(*this, "π₀i() r=∅");
    let [fi, x] = p.at(r.back()++);
    f.at(fi)(*this, x); return *this; }
#else
  π0int &operator()()
  { let [fi, x] = p[r.back()++];
    f[fi](*this, x); return *this; }
#endif
};


#if τdebug_iostream
π0TG O &operator<<(O &s, π0T(π0int) const &i)
{
  s << "π₀i qs=" << i.q->size()
    << " r=";
  if (!i.r.empty())
    for (iN j = i.r.size() - 1; j >= 0; --j)
      s << i.r[j] << " ";
  return s << i.h << std::endl;
}
#endif


}

#include "pi0-end.hh"


#endif
