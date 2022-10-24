#ifndef τπ0int_h
#define τπ0int_h


#include "types.hh"
#include "utf9.hh"

#include "pi0-types.hh"
#include "pi0-gc.hh"
#include "pi0-abi.hh"
#include "pi0-pgm.hh"


#include "begin.hh"

namespace τ
{


// NOTE: π0int : π0sv only for notational purposes; we never use π0int
// as a stack view.

struct π0int : π0sv
{
  π0abi const &a;   // ABI (native functions)
  π0pgm const  p;   // bytecode program
  π0h         &h;   // data stack + local frames
  V<π0bi>      r;   // return stack
  π0hdf        f;   // frame stack
  π0hds        d;   // base data stack
  π0sv        *dv;  // current data stack view

  π0int(π0abi const &a_, π0pgm &&p_, π0h &h_)
    : a(a_), p(std::move(p_)), h(h_), f(h), d(h), dv(&d)
  { A(p.a.v() == a.v(), "π₀ ABI mismatch: " << p.a.v() << " ≠ " << a.v()); }


  π0int &run(uN l)
    { let n = r.size();
      r.push_back(l);
      while (r.size() > n) step();
      return *this; }


  // Stack-view accessors, used by bytecode functions
  π0sv        *up()     const { return nullptr; }
  uN         size()     const { return dv->size(); }
  π0r  operator[](uN i) const { return (*dv)[i]; }
  void operator<<(π0r x)      { *dv << x; }
  void       drop(uN n)       { dv->drop(n); }

  π0int &spush() { dv = new π0hss{h, *dv};              return *this; }
  π0int &spop()  { let v = dv; dv = dv->up(); delete v; return *this; }


  template<o9mapped T>
  π0int &dpush(T const &x) { *this << (h << o9(x)); return *this; }

  template<class T = i9> T dpop() { let r = h[(*dv)[0]]; drop(1); return Sc<T>(r); }


  // Frame accessors
  π0int &fpush(uN s) { f.push(s); return *this; }
  π0int &fpop()      { f.pop();   return *this; }
  π0r   &fi(uN i)    { return f[i]; }


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
O &operator<<(O &s, π0int const &i)
{
  s << "π₀i qs=" << i.p.q.size()
    << " r=";
  if (!i.r.empty())
  { for (iN j = i.r.size() - 1; j >= 0; --j)
      s << i.r[j] << " ";
    let [fi, x] = i.p.p[i.r.back()];
    s << " " << i.p.a.n[fi] << "'" << x; }
  s << std::endl;
  for (uN j = 0; j < i.size(); ++j)
    s << "  [" << j << "]\t" << i.h[i[j]] << std::endl;
  return s;
}
#endif


}

#include "end.hh"


#endif
