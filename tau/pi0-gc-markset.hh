#ifndef τπ0_gc_markset_h
#define τπ0_gc_markset_h


#include <algorithm>


#include "types.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"
#include "pi0-gc-heapspace.hh"

#include "pi0-gc-begin.hh"

namespace τ
{


π0TGs π0ms  // mark-set for one generation
{
  π0TS;
  π0T(π0h)                 &h;
  π0hg                const g;   // generation being marked
  V<π0T(π0r)>               m;   // marked refs (internal + external)
  M<π0T(π0r), S<π0T(π0r)>>  r;   // internal references


  void mi(π0T(π0r) x, π0T(π0r) y)  // mark internal reference x → y
  { me(y); r[y].insert(x); }

  void me(π0T(π0r) x)              // mark external reference to x
  { if (x.g() == g && !r.contains(x))
    { r[x] = {};
      m.push_back(x);
      for (let i : h[x].flags())
        // NOTE: we track references in all generations in case older
        // generations end up being collected here -- we just don't
        // trace those older-gen objects yet
        if (i.is_πref()) mi(x, π0T(π0r)(i)); } }
};


π0TGs π0gs  // splice map for one generation
{
  π0TS;
  π0T(π0h)    &h;
  π0hg const   g;


  π0gs(π0T(π0ms) &ms) : h(ms.h), g(ms.g)
  { std::sort(ms.m.begin(), ms.m.end());
    V<π0T(π0r)> ri;
    for (let x : ms.m)
      for (let i : h[x].flags())
        if (i.is_πref())
        { let y = π0T(π0r)(i);
          if (y.g() == g && ms.r[y].size() == 1 && !c(ms.m, y))
            ri.push_back(h(g, i)); }

    // TODO: translate inlines into splices
    // TODO: remove inlined things from root set
  }

  bool c(V<π0T(π0r)> &m, π0T(π0r) x)
  { let r = std::upper_bound(m.begin(), m.end(), x);
    return r != m.end() && h[x].a < h[*r].next().a; }
};


}

#include "pi0-gc-end.hh"


#endif
