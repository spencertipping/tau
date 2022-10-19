#ifndef τπ0_gc_markset_h
#define τπ0_gc_markset_h


#include <algorithm>
#include <cstring>


#include "debug.hh"
#include "types.hh"
#include "utf9-types.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"
#include "pi0-gc-heapspace.hh"

#include "pi0-gc-begin.hh"


#if !defined(τπ0debug_gc_postcopy_verify)
# define τπ0debug_gc_postcopy_verify τdebug
#endif


namespace τ
{


// NOTE: π0ms and π0gs (defined below) are split deliberately to reflect
// the state-wall between them. Splicing and inlining require us to
// analyze the whole heap at once and look at objects in their allocation
// order. So we don't have anything like incremental marking.


π0TGs π0ms  // mark-set for one generation
{
  π0TS;
  π0T(π0h)       &h;
  π0hg      const g;  // generation being marked
  V<π0R>          m;  // marked refs (internal + external)
  M<π0R, S<π0R>>  r;  // internal references

  // TODO: remove r from this class, replace m with sorted set

  // NOTE: no need to chase down references-to-references because we will
  // always inline small objects (like references)
  void mi(π0R x, π0R y)  // mark internal reference x → y
  { me(y);
    h.mark(y);           // important: mark into other generations
    r[y].insert(x); }

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

  bool operator< (π0T(π0gS) const &x) const { return o <  x.o; }
  bool operator<=(π0T(π0gS) const &x) const { return o <= x.o; }
};


π0TGs π0gSs              // multilevel splice set
{
  π0TS;
  π0T(π0h)     &h;
  π0hg const    g;
  V<π0T(π0gS)> &ss;      // splice points
  M<π0R, uN>   &nsf;     // new size+flag of each old-heap object
  V<i9>         s;       // container start points
  V<ζp>         e;       // contained end points
  V<bool>       f;       // flags
  iN            Δs = 0;  // running size differences

  ~π0gSs() { while (*this) pop(); }

  operator   bool()     const { return !e.empty(); }
  ζp          end()     const { return  e.back(); }
  bool operator<=(ζp x) const { return *this && end() <= x; }
  i9   operator()(i9 x)       { while (*this <= x.a) pop(); return x; }
  void       flag()           { f.assign(f.size() - 1, true); }

  void operator<<(i9 i)
  { s.push_back(i);
    e.push_back(i.next());
    f.push_back(false); }

  π0R inl(i9 i, i9 t)  // returns old-heap ref to i9 reference
  { let r   = h(g, i);
    let d   = h(g, t);
    let s1  = i.osize();
    let dsf = nsf.contains(d) ? nsf[d] : t.osize() << 1 | t.flagged();
    let s2  = dsf >> 1;
    Δs += s2 - s1;
    ss.push_back({r, s1, t, s2, 0});
    if (dsf & 1) flag();
    return r; }

  void pop()
  { let i = s.back(); s.pop_back();
    let b = f.back(); f.pop_back();
    e.pop_back();
    if (*this) f.assign(f.size() - 1, f.back() | b);

    let ia  = h(g, i);
    let is  = i.size();
    let isb = u9sb(i.stype());
    let nsb = u9sb(u9sq(is + Δs));
    nsf[ia] = is + Δs << 1 | b;
    ss.push_back({ia, isb, Rc<ζp>(is + Δs), nsb, 2 | b});
    Δs += nsb - isb; }
};


π0TGs π0gs  // splice map for one generation
{
  π0TS;
  sletc csb = u9sb(u9s::v64);  // buffer bytes per container size splice

  π0T(π0h)    &h;
  π0hg   const g;
  V<π0R>       m;    // root marked objects
  S<π0R>       ri;   // inlined references
  V<π0T(π0gS)> s;    // splice points
  B            b;    // buffer for new control/size byte patches
  M<π0R, uN>   nsf;  // new size + flag (flag = lsb) for each object
  uN           lss;  // live-set size

  π0gs(π0T(π0ms) &&ms)
  : h(ms.h), g(ms.g), m(std::move(ms.m)), lss(0)

  { std::sort(m.begin(), m.end());

    // NOTE: immutability guarantees that forward references don't exist,
    // so we're guaranteed that each x in the for-loop below has not been
    // inlined by the time we visit it -- although it's allowed to inline
    // things we've visited on previous iterations.
    uN css = 0;  // number of container size splices
    for (let x : ms.m)
    { π0T(π0gSs) ss{h, g, s, nsf};
      for (let i : h[x].flags())
        // If we inline an object, that object is now covered by
        // whichever thing it's inlined by; so we don't need it to
        // be a part of the root set anymore. However, we can't just
        // remove it from the root set because the inline may itself
        // contain other inlines, and those should still be "contained".
        // So we keep a separate "we've inlined this" set and check
        // that before tenuring/copying a potential root object.
        //
        // Three states here:
        //   flagged non-reference   -- push container
        //   reference to be inlined -- flag if referent is flagged
        //   reference not to inline -- always flag
        if      (!ss(i).is_πref())    ss << i, ++css;
        else if (let d = ii(i, ms.r)) ri.insert(ss.inl(i, h[d]));
        else                          ss.flag(); }

    // Convert inlined size patches, stored in .a, into buffer references
    // that can be spliced in the normal memcpy way; inlined patches have
    // .c != 0. This loop is the last point that b is modified.
    //
    // Full spec: new size = .a, flag = .c & 1
    b.reserve(css * csb);
    for (auto &x : s)
      if (x.c)
      { let ns = Rc<uN>(x.a);
        u9ws(x.a = b.data() + b.size(), 0,
             i9{h[x.o]}.type(), ns, x.c & 1);
        b.resize(b.size() + csb); }

    // Sort and fix up cumulative offsets for patching -- note that these
    // offsets span multiple objects.
    iN cd = 0;
    std::sort(s.begin(), s.end());
    for (auto &x : s) x.c = cd += x.s - x.d;

    // Finally, calculate the live-set size so we can easily report it.
    for (let x : m) if ((*this)[x]) lss += newsize(x); }


  // Returns the referent if the given i9 ref should be inlined
  //
  // FIXME: this logic should work, but we probably don't need or want
  // rs[] here; we should be able to inline multiply-referenced objects
  // without issue. The big consideration is whether the object is
  // contained within another marked object.
  //
  // I believe we can inline if same-gen, not contained, and not in ri
  // (i.e. not claimed by another object wanting to inline it).
  π0R ii(i9 r, M<π0R, S<π0R>> &rs) const
  { let d = π0R(r);
    return d.g() == g && rs[d].size() == 1 && !ci(d)
         ? d
         : π0R(); }

  bool ci(π0R x) const
  { let i = std::lower_bound(m.begin(), m.end(), x);
    return i != m.end() && *i != x && h[x].a < h[*i].next().a; }


  // Root-set iteration -- if [] is true, then you need to copy the
  // object into newspace
  typename V<π0R>::const_iterator begin() const { return m.begin(); }
  typename V<π0R>::const_iterator end()   const { return m.end(); }
  bool operator[](π0R a)                  const { return !ri.contains(a); }


  typename V<π0T(π0gS)>::const_iterator rpt(π0R x) const
  { return std::lower_bound(s.begin(), s.end(), π0gS{x}); }

  uN newsize(π0R x) const
  { return nsf.contains(x) ? nsf.at(x) >> 1 : h[x].osize(); }


  // Patch a reference to offset x within root r by returning x', the new
  // offset. It's assumed that you know r', the newspace root location.
  π0ha patch(π0R r, π0ha x) const
  { let i = rpt(r);
    if (i == s.end()) return x;
    let j = std::lower_bound(i, s.end(), π0gS{r + x});
    return x - (*i).c + (*j).c; }
};


π0TGs π0gso9 : virtual o9V  // spliced heap-copy out from a generation
{
  π0TS;
  π0T(π0gs) &gs;
  π0R        o;
  π0gso9(π0T(π0gs) &gs_, π0R o_) : gs(gs_), o(o_) {}

  uN size ()     const { return gs.newsize(o); }
  uN write(ζp m) const
  { // NOTE: there may be some performance advantage to merging memcpy()
    // calls when the offsets line up, but it adds complexity and
    // probably isn't significant.
    let  i  = gs.h[o];
    let  os = i.osize();
    let  oe = o + os;  // end of original object
    uN   c  = 0;       // copy source (relative to original object)
    uN   d  = 0;       // copy destination (relative to m)
    auto s  = gs.rpt(o);
    if (s != gs.s.end() && *s < π0gS{o}) ++s;

    while (c < os && s != gs.s.end() && *s < π0gS{oe})
    { // Right now we have o + c ≤ *s < oe, which means there's a splice
      // between the copy source point and the end of the input object.
      let l = (*s).o - (o + c);  // verbatim-copy length
      std::memcpy(m + d,     i.a + c, l);
      std::memcpy(m + d + l, (*s).a,  (*s).s);
      c += l + (*s).d;
      d += l + (*s).s;
      ++s; }

    std::memcpy(m + d, i.a + c, os - c);
    if (τπ0debug_gc_postcopy_verify)
      A(i9{m}.verify(), "π₀gso9 !v, i = " << i << ", o = " << i9{m});
    return 0; }
};


π0TGn struct o9_<π0T(π0gso9)> { sletc v = true; };


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
