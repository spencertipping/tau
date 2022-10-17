#ifndef τπ0_gc_markset_h
#define τπ0_gc_markset_h


#include "types.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"
#include "pi0-gc-heapspace.hh"
#include "pi0-gc-splice.hh"

#include "pi0-gc-begin.hh"

namespace τ
{


π0TGs π0ms  // mark-set for one generation
{
  π0TS;
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

  void plan(π0T(π0gs) &s)                // construct inline splice plan
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


}

#include "pi0-gc-end.hh"


#endif
