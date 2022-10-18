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


π0TGs π0gSs           // splice set
{
  π0TS;
  π0T(π0h)  &h;
  π0hg const g;
  V<π0gS>   &ss;      // splice points
  V<i9>      s;       // container start points
  V<ζp>      e;       // contained end points
  V<bool>    f;       // flags
  iN         Δs = 0;  // running size differences

  operator   bool()     const { return !e.empty(); }
  ζp          end()     const { return  e.back(); }
  bool operator<=(ζp x) const { return *this && end() <= x; }
  void operator()(ζp x)       { while (*this <= x) pop(); }
  void       flag()           { f.assign(f.size() - 1, true); }

  void operator<<(i9 i)
  { s.push_back(i);
    e.push_back(i.next());
    f.push_back(false); }

  void pop()
  { let i = s.back(); s.pop_back();
    let b = f.back(); f.pop_back();
    e.pop_back();
    if (*this) f.assign(f.size() - 1, f.back() | b);

    let ia  = h(g, i);
    let is  = i.size();
    let isb = u9sb(i.stype());
    let nsb = u9sb(u9sq(is + Δs));
    ss.push_back({ia, isb, Rc<ζp>(is + Δs), nsb, 2 | b});
    Δs += nsb - isb; }


  void inl(π0R r, i9 t)
  { let s1 = h[r].osize();
    let s2 = t.osize();
    ss.push_back({r, s1, t, s2, 0});
    Δs += s2 - s1; }
};


π0TGs π0gs  // splice map for one generation
{
  π0TS;
  sletc csb = u9sb(u9s::v64);  // bytes per container size splice

  π0T(π0h)  &h;
  π0hg const g;
  S<π0R>     m;    // root marked objects
  V<π0gS>    s;    // splice points
  B          b;    // buffer for new control/size byte patches
  uN         lss;  // live-set size

  π0gs(π0T(π0ms) &ms)
  : h(ms.h), g(ms.g), m(ms.m.begin(), ms.m.end()), lss(0)

  { uN css = 0;    // number of container size splices

    // NOTE: when building up the splice set, we set c = 0 for regular
    // splices and c = 2 or 3 for size splices. For size splices,
    // π0gS.a is a uN, not a ζp, and it's the container's new size.
    std::sort(ms.m.begin(), ms.m.end());
    for (let x : ms.m)
    { π0T(π0gSs) ss{h, g, s};
      let i = h[x];
      for (let j : i.flags())
      { ss(i.a);
        if (j.is_πref())
        { let d = π0R(j);
          if (ii(d, ms.m, m, ms.r))
          { let r = h(g, j);
            ss.inl(r, h[d]);

            // If we inline an object, that object is now covered by
            // whichever thing it's inlined by; so we don't need it to
            // be a part of the root set anymore.
            //
            // TODO: do we need to leave it in the root set and track
            // inlines? If we don't, we may think its children are not
            // contained, which means they are falsely eligible for
            // inlining
            m.erase(r); }

          else
            ss.flag(); }
        else
          ss << j; }
      ss(i.next().a);
      lss += i.osize() + ss.Δs; }

    // Convert inlined size patches (as per above) into buffer references
    // that can be spliced in the normal memcpy way
    b.reserve(css * csb);
    for (auto &x : s)
      if (x.c)
      { let ns = Rc<uN>(x.a);
        u9ws(x.a = b.data() + b.size(), 0,
             i9{h[x.o]}.type(), ns, x.c & 1);
        b.resize(b.size() + csb); }

    // Sort and fix up cumulative offsets for patching -- note that these
    // offsets span multiple objects
    std::sort(s.begin(), s.end());
    iN cd = 0;
    for (auto &x : s) x.c = cd += x.s - x.d; }


  // TODO: this is a little too strict; we can inline some objects
  // that have multiple references (but for now it's fine if we don't)
  bool ii(π0R d, V<π0R> &m, S<π0R> &s, M<π0R, S<π0R>> &rs) const
  { return d.g() == g && rs[d].size() == 1 && !ci(m, s, d); }


  // True if x is contained by another element of the root set (which
  // must be sorted)
  bool ci(V<π0R> &m, S<π0R> &s, π0R x) const
  { if (s.contains(x)) return false;
    let r = std::lower_bound(m.begin(), m.end(), x);
    return r != m.end() && h[x].a < h[*r].next().a; }
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
