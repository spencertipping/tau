#ifndef τπint_h
#define τπint_h


#include "types.hh"
#include "utf9.hh"

#include "pi-types.hh"
#include "pi-gc.hh"
#include "pi-abi.hh"
#include "pi-pgm.hh"

#include "Phi.hh"


#include "begin.hh"

namespace τ
{


// NOTE: πint : πsv only for notational purposes; we never use πint
// as a stack view within another πint.

struct πint : πsv
{
  // NOTE: this can be an upper bound; it must be large enough for a
  // reference on any architecture
  sletc πo9r_size = 11;

  πabi const     &a;   // ABI (native functions)
  SP<πpgm const>  p;   // bytecode program
  Φ              &f;   // shared Φ boundary
  πh             &h;   // shared heap (cached from Φ)
  SP<πhgs>        g;   // shared globals

  V<πbi>          rs;  // return stack
  πhdf            fs;  // frame stack
  πhds            ds;  // base data stack
  πsv            *dv;  // current data stack view

  // NOTE: this is a fork-constructor, not a copy-constructor (stacks are
  // empty in the destination)
  πint(πint const &i, SP<πhgs> g_ = nullptr)
    : a(i.a), p(i.p), f(i.f), h(i.h), g(g_ ? g_ : i.g), fs(h), ds(h), dv(&ds) {}

  πint(πabi const &a_, SP<πpgm const> p_, Φ &f_, SP<πhgs> g_)
    : a(a_), p(p_), f(f_), h(f_.ph), g(g_), fs(h), ds(h), dv(&ds)
    { if (p) A(p->a.v() == a.v(), "π₀ ABI mismatch: " << p->a.v() << " ≠ " << a.v()); }

  ~πint() { while (dv != &ds) spop(); }


  πint &run(πbi l = 0)
    { let n = rs.size();
      rs.push_back(l ? l : p->e);
      while (rs.size() > n) step();
      return *this; }


  πhgs &gs() const { return *g; }


  // Stack-view accessors, used by bytecode functions
  πsv         *up()     const { return nullptr; }
  uN         size()     const { return dv->size(); }
  i9   operator[](uN i) const { return (*dv)[i]; }
  void operator<<(πr x)       { *dv << x; }
  void       drop(uN n)       { dv->drop(n); }

  πint &spush(uN n = 0) {             dv = new πhss{h, *dv, n}; return *this; }
  πint &spop()          { let v = dv; dv = dv->up(); delete v;  return *this; }


  // TODO: allocate small objects directly onto the stack
  // to avoid GC
  //
  // TODO: specialize this for i9 to push a reference to on-heap
  // objects that are above the inlining threshold
  //
  // TODO: make the above "refer to existing" functionality generic
  template<o9mapped T>
  πint &dpush(T const &x)
    { let o = o9(x);
      let s = o.size();  h.res(s <= h.is ? s : s + πo9r_size);
      let r = h << o;
      if (s <= h.is) *this << r;
      else           *this << (h << πo9r(r));
      return *this; }

  i9  dpop() { let r = (*dv)[0]; drop(1); return r.deref(); }
  πbi bpop() { return Sc<πbi>(dpop()); }


  // Frame accessors
  πint &fpush(uN s) { fs.push(s); return *this; }
  πint &fpop()      { fs.pop();   return *this; }
  πr   &fi(uN i)    { return fs[i]; }


#if τπdebug_bounds_checks
  πint &step()
  { let [fi, x] = p->p.at(rs.back()++);
    a.f.at(fi)(*this, x); return *this; }
#else
  πint &step()
  { let [fi, x] = p->p[rs.back()++];
    a.f[fi](*this, x); return *this; }
#endif
};


struct πrsf  // π₀int return stack floor
{
  πint const &i;
  uN          rn;

  πrsf(πint &i_) : i(i_), rn(i.rs.size()) {}
  operator bool() const { return i.rs.size() >= rn; }
};


#if τdebug_iostream
O &operator<<(O &s, πint const &i)
{
  s << "π₀i qs=" << i.p->q.size()
    << " r[" << i.rs.size() << "]=";
  if (!i.rs.empty())
  { for (iN j = i.rs.size() - 1; j >= 0; --j)
      s << i.rs[j] << " ";
    let &[fi, x] = i.p->p[i.rs.back()];
    s << " " << i.p->a.n[fi] << "'" << x; }
  s << std::endl;
  for (uN j = 0; j < i.size(); ++j)
    s << "  [" << j << "]\t" << i[j] << std::endl;
  return s;
}
#endif


}

#include "end.hh"


#endif
