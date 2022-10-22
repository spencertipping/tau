#ifndef τπ0int_h
#define τπ0int_h


#include "types.hh"
#include "numerics.hh"
#include "Lambda.hh"
#include "zeta.hh"
#include "utf9.hh"

#include "pi0-types.hh"
#include "pi0-gc.hh"
#include "pi0-abi.hh"


#include "pi0-begin.hh"

namespace τ
{


// NOTE: π0int : π0sv only for notational purposes; we never use π0int
// as a stack view.

π0TGs π0int : π0T(π0sv)
{
  π0TS;
  π0T(π0abi) const &a;   // ABI (native functions)
  π0T(π0p)   const  p;   // bytecode program
  π0T(π0h)         &h;   // data stack + local frames
  V<uN>             r;   // return stack
  π0T(π0hdf)        f;   // frame stack
  π0T(π0hds)        d;   // base data stack
  π0T(π0sv)        *dv;  // current data stack view

  π0int(π0T(π0abi) const &a_, π0T(π0p) &&p_, π0T(π0h) &h_)
    : a(a_), p(std::move(p_)), h(h_), f(h), d(h), dv(&d)
  { A(p.v == a.v(), "π₀ ABI mismatch: " << p.v << " ≠ " << a.v()); }


  π0int &run(uN l)
    { let n = r.size();
      r.push_back(l);
      while (r.size() > n) step();
      return *this; }


  // Stack-view accessors, used by bytecode functions
  π0T(π0sv)   *up()     const { return nullptr; }
  uN         size()     const { return dv->size(); }
  π0R  operator[](uN i) const { return (*dv)[i]; }
  void operator<<(π0R x)      { *dv << x; }
  void       drop(uN n)       { dv->drop(n); }

  π0int &spush() { dv = new π0T(π0hss){h, *dv};         return *this; }
  π0int &spop()  { let v = dv; dv = dv->up(); delete v; return *this; }


  template<o9mapped T>
  π0T(π0int) &dpush(T const &x) { *this << (h << o9(x)); return *this; }

  i9 dpop() { let r = h[(*dv)[0]]; drop(1); return r; }


  // Frame accessors
  π0int &fpush(uN s) { f.push(s); return *this; }
  π0int &fpop()      { f.pop();   return *this; }
  π0R   &fi(uN i)    { return f[i]; }


  // Native frames are always created externally because their lifetime
  // is managed by the caller


#if τπ0debug_bounds_checks
  π0int &step()
  { let [fi, x] = p.p.at(r.back()++);
    a.f.at(fi)(*this, x); return *this; }
#else
  π0int &step()
  { let [fi, x] = p.p[r.back()++];
    a.f[fi](*this, x); return *this; }
#endif
};


#if τdebug_iostream
π0TG O &operator<<(O &s, π0T(π0int) const &i)
{
  s << "π₀i qs=" << i.p.q.size()
    << " r=";
  if (!i.r.empty())
    for (iN j = i.r.size() - 1; j >= 0; --j)
      s << i.r[j] << " ";
  return s;
}
#endif


}

#include "pi0-end.hh"


#endif
