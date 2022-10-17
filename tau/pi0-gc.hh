#ifndef τπ0_gc_h
#define τπ0_gc_h


// Specialize templates when parsing with emacs/LSP; this #if
// will be skipped in production builds
#if !defined(τ_h)
# define τπ0debug_specialize_templates 1
#endif


#include "debug.hh"

#include "Sigma.hh"
#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc-splice.hh"

#include "pi0-gc-begin.hh"

namespace τ
{


π0TGs π0hs  // a single GC heap-space (generation)
{ π0TS;
  B h;

  template<O9 T>
  π0T(π0r) operator<<(T const &x)
    { return h.size() + x.size() > h.capacity() ? π0T(π0r)() : h << x; }

  i9 operator[](uN x) { return i9{h.data() + x}; }
};


π0TGs π0hv;
π0TGs π0ms;

π0TGs π0h   // a multi-generational heap (Gb = generation bits)
{ π0TS;
  sletc gn = Sc<uN>(1) << Gb;

  π0T(π0hs)     gs[gn];    // generations; 0 = newest
  π0T(π0ms)    *ms[gn];    // during GC, mark-set for each gen
  π0T(π0gs)    *ss[gn];    // during GC, splice plan for each gen
  ΣΘΔ          gΘ;
  S<π0T(π0hv)*> vs;        // views that comprise the root set
  bool         ga;        // is GC active

  π0h () : ga{false} {}
  ~π0h() { A(vs.empty(), "~π0h vs nonempty (will segfault on ~π0hv)"); }


  i9 operator[](π0T(π0r) x) { return gs[x.g()][x.a()]; }

  π0T(π0r) operator()(ζp x) const
    { for (uN i = 0; i < gn; ++i)
      { let y = (*this)(i, x); if (!y.ω()) return y; }
      return π0T(π0r)(); }

  π0T(π0r) operator()(uN g, ζp x) const
    { let &b = gs[g].h;
      return x >= b.data() && x < b.data() + b.size()
           ? π0T(π0r)(g, x - b.data())
           : π0T(π0r)(); }


  template<O9 T>
  π0T(π0r) operator<<(T const &x)
    { let a = gs[0] << x;
      if (!a.ω()) return gar(0, a);
      gc(0, x.size());    // invariant: post-GC we will have space for x
      return gar(0, gs[0] << x); }


  void     gc  (uN g, uN s);       // gc a specific generation downwards
  π0T(π0r) move(π0T(π0r)) const;   // used by π0hv to translate old → new ext refs
  void     mark(π0T(π0r)) const;   // used by π0hv to externally mark a reference
};


π0TGs π0ms  // mark-set for one generation
{ π0TS;
  π0T(π0h)                 &h;
  uNc                       g;  // generation being marked
  V<π0T(π0r)>               m;  // marked refs (internal + external)
  M<π0T(π0r), S<π0T(π0r)>>  r;  // internal references

  void trace(π0T(π0r) x)
    { if (x.g() == g)
        for (let i : h[x].flags())
          // NOTE: we track references in all generations in case older
          // generations end up being collected here -- we just don't
          // trace those older-gen objects yet
          if (let y = i.π()) mark_internal(x, π0T(π0r)(y)); }

  void mark_internal(π0T(π0r) x, π0T(π0r) y) { mark_external(y); r[y].insert(x); }
  void mark_external(π0T(π0r) x)
    { if (x.g() == g && !r.contains(x))
        r[x] = {}, m.push_back(x), trace(x); }

  void plan(π0T(π0gs) &s)                 // construct inline splice plan
    { V<bool> c; c.resize(m.size(), false);
      std::sort(m.begin(), m.end());

      for (uN i = 0; i < m.size();)      // solve for roots/containers
      { let e = h[m[i++]].end().a;
        while (i < m.size() && h[m[i]].a < e) c[i++] = true; }

      for (uN i = 0; i < m.size(); ++i)  // inline single, same-gen refs
        if (!c[i]) plan_ref_inlines(s, c, m[i]); }

  void plan_ref_inlines(π0T(π0gs) &s, V<bool> &c, π0T(π0r) x)
    { for (let i : h[x].flags())
        if (let y = i.π())
        { let z = π0T(π0r)(y);
          // Invariant: z will be marked here, meaning it will have an
          // entry in r[]. This means we can determine how many other
          // references exist; if it's singly-referenced, we can inline
          // it here.
          if (z.g() == g && r[z].size() == 1)
            s.plan_inline(x); } }
};


π0TGs π0hv  // heap view: a part of the root set
{ π0TS;
  π0T(π0h) &h;
  π0hv(π0T(π0h) &h_) : h(h_) {}
  virtual ~π0hv() { h.vs.erase(this); }
  virtual void mark() = 0;  // mark everything in this view
  virtual void move() = 0;  // move everything in this view
};


π0TG void π0T(π0h)::gc(uN g, uN s)
{
  TODO("gc");
}


}

#include "pi0-gc-end.hh"


#endif
