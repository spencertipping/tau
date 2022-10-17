#ifndef τπ0_gc_h
#define τπ0_gc_h


#include "Sigma.hh"
#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc-splice.hh"

#include "begin.hh"

namespace τ
{


struct π0hg  // a single GC generation
{
  B h;

  template<O9 T>
  π0r operator<<(T const &x)
    { return h.size() + x.size() > h.capacity() ? π0hω : h << x; }

  i9 operator[](uN x) { return i9{h.data() + x}; }
};


template<uN Gb> struct π0hv;
template<uN Gb> struct π0ms;

template<uN Gb = 2>
struct π0h   // a multi-generational heap (Gb = generation bits)
{
  sletc gn   = Sc<π0r>(1) << Gb;
  sletc ghms = τwordsize - Gb;
  sletc ghm  = gn - 1 << ghms;
  sletc gam  = ~ghm;
  static_assert(ghm | gam == π0hω);
  static_assert(ghm & gam == 0);

  static ic uN  rg (π0r x)      { return x >> ghms; }  // ref → generation
  static ic uN  ra (π0r x)      { return x  & gam; }   // ref → address
  static ic π0r gar(uN g, uN a) { return g << ghms | a; }


  π0hg         gs[gn];    // generations; 0 = newest
  π0ms<Gb>    *ms[gn];    // during GC, mark-set for each gen
  π0gs        *ss[gn];    // during GC, splice plan for each gen
  ΣΘΔ          gΘ;
  S<π0hv<Gb>*> vs;        // views that comprise the root set
  bool         ga;        // is GC active

  π0h () : ga{false} {}
  ~π0h() { A(vs.empty(), "~π0h vs nonempty (will segfault on ~π0hv)"); }


  i9 operator[](π0r x) const { return gs[rg(x)][ra(x)]; }

  π0r operator()(ζp x) const
    { for (uN i = 0; i < gn; ++i)
      { let y = (*this)(i, x); if (y != π0hω) return y; }
      return π0hω; }

  π0r operator()(uN g, ζp x) const
    { let &b = gs[g].h;
      return x >= b.data() && x < b.data() + b.size()
           ? gar(g, x - b.data())
           : π0hω; }


  template<O9 T>
  π0r operator<<(T const &x)
    { let a = gs[0] << x;
      if (a != π0hω) return gar(0, a);
      gc(0, x.size());    // invariant: post-GC we will have space for x
      return gar(0, gs[0] << x); }


  void gc  (uN g, uN s);  // gc a specific generation downwards
  π0r  move(π0r) const;   // used by π0hv to translate old → new ext refs
  void mark(π0r) const;   // used by π0hv to externally mark a reference
};


template<uN Gb>
struct π0ms  // mark-set for one generation
{
  π0h<Gb>        &h;
  uN              g;      // generation being marked
  V<π0r>          m;      // marked refs (internal + external)
  M<π0r, S<π0r>>  r;      // internal references

  void trace(π0r x)
    { if (h.rg(x) == g)
      { let i = h[x];
        if (i.flagged())
          if      (let y = i.π()) mark_internal(x, y);
          else if (u9c[i.type()]) for (let y : i) trace(h(g, y)); } }

  void mark_internal(π0r x, π0r y) { mark_external(y); r[y].insert(x); }
  void mark_external(π0r x)
    { if (h.rg(x) == g && !r.contains(x))
        r[x] = {}, m.push_back(x), trace(x); }

  void plan(π0gs &s)
    { V<bool> c; c.resize(m.size(), false);
      std::sort(m.begin(), m.end());
      for (uN i = 0; i < m.size();)
      { let e = h[m[i++]].end().a;
        while (i < m.size() && h[m[i]].a < e) c[i++] = true; }
      for (uN i = 0; i < m.size(); ++i)
        if (!c[i]) plan_ref_inlines(s, c, m[i]);

      TODO("plan tenuring"); }

  void plan_ref_inlines(π0gs &s, V<bool> &c, uN x)
    { let i = h[x];


    }
};


template<uN Gb>
struct π0hv  // heap view: a part of the root set
{
  π0h<Gb> &h;
  virtual ~π0hv() { h.vs.erase(this); }
  virtual void mark() = 0;  // mark everything in this view
  virtual void move() = 0;  // move everything in this view
};


template<uN Gb>
void π0h<Gb>::gc(uN g, uN s)
{
  TODO("gc");
}


}

#include "end.hh"


#endif
