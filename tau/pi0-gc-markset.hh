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
  π0T(π0h)       &h;
  π0hg      const g;   // generation being marked
  V<π0R>          m;   // marked refs (internal + external)
  M<π0R, S<π0R>>  r;   // internal references


  // NOTE: no need to chase down references-to-references because we will
  // always inline small objects (like references)

  void mi(π0R x, π0R y)  // mark internal reference x → y
  { me(y); r[y].insert(x); }

  void me(π0R x)         // mark external reference to x
  { if (x.g() == g && !r.contains(x))
    { r[x] = {};
      m.push_back(x);
      for (let i : h[x].flags())
        // NOTE: we track references in all generations in case older
        // generations end up being collected here -- we just don't
        // trace those older-gen objects yet
        if (i.is_πref()) mi(x, π0R(i)); } }
};


π0TGs π0gS  // splice point
{
  π0TS;
  π0R o;    // origin point
  uN  d;    // deletion size
  ζp  a;    // copy-from address
  uN  s;    // splice size
  iN  c;    // cumulative displacement from original

  bool operator<(π0T(π0gS) const &x) const { return o < x.o; }
};


π0TGs π0gs  // splice map for one generation
{
  π0TS;
  sletc csb = u9sb(u9s::v64);  // bytes per container size splice

  π0T(π0h)  &h;
  π0hg const g;
  S<π0R>     m;  // root marked objects
  V<π0gS>    s;  // splice points
  B          b;  // buffer for new control/size byte patches

  π0gs(π0T(π0ms) &ms) : h(ms.h), g(ms.g), m(ms.m.begin(), ms.m.end())
  { uN css = 0;  // number of container size splices
    std::sort(ms.m.begin(), ms.m.end());

    // NOTE: when building up the splice set, we set c = 0 for regular
    // splices and c = 1 for size splices. For size splices, π0gS.a is
    // an iN, not a ζp, and it's the new size + flag state for the
    // container.
    for (let x : ms.m)
    { V<T<ζp, ζp, bool>> cs;
      iN                 Δs = 0;
      for (let i : h[x].flags())
      { while (!cs.empty() && i.a >= std::get<1>(cs.back()))
        { let [j, _, f] = cs.back();
          cs.pop_back();
          if (!cs.empty()) std::get<2>(cs.back()) |= f;
          let ja = h(g, j);
          let js = u9sb(i9{j}.stype());
          s.push_back({ja, js, Rc<ζp>(js + Δs), u9sb(u9sq(js + Δs)), 2 | f}); }
        if (i.is_πref())
        { let y = π0R(i);
          // TODO: this is a little too strict; we can inline some objects
          // that have multiple references
          if (y.g() == g && ms.r[y].size() == 1 && !c(ms.m, y))
          { let z  = h(g, i);
            let s1 = i.osize();
            let s2 = h[y].osize();
            s.push_back({z, s1, i, s2, 0});
            Δs += s2 - s1;
            m.erase(z); }
          else
            std::get<2>(cs.back()) = true; }
        else
          cs.push_back({i, i.next(), false}); }
      while (!cs.empty())
      { let [j, _, f] = cs.back();
        cs.pop_back();
        if (!cs.empty()) std::get<2>(cs.back()) |= f;
        let ja = h(g, j);
        let js = u9sb(i9{j}.stype());
        s.push_back({ja, js, Rc<ζp>(js + Δs), u9sb(u9sq(js + Δs)), 2 | f}); } }

    // Fix up revised sizes
    b.reserve(css * csb);
    for (auto &x : s)
      if (x.c)
      { let ns = Rc<uN>(x.a);
        x.a = b.data() + b.size();
        u9ws(x.a, 0, i9{h[x.o]}.type(), ns, x.c & 1);
        b.resize(b.size() + csb); }

    // Sort and fix up cumulative offsets for patching
    std::sort(s.begin(), s.end());
    iN cd = 0;
    for (auto &x : s) x.c = cd += x.s - x.d; }

  bool c(V<π0R> &m, π0R x)
  { let r = std::upper_bound(m.begin(), m.end(), x);
    return r != m.end() && h[x].a < h[*r].next().a; }

  uN size() const
  { TODO("size");
  }
};


#if τdebug_iostream
π0TG O &operator<<(O &s, π0T(π0gS) const &x)
{
  return s << "π₀gS:" << x.o << "-" << x.d << ":" << x.a << "+" << x.s
           << "=" << x.c;
}
#endif


}

#include "pi0-gc-end.hh"


#endif
