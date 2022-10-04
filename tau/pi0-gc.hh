#ifndef τπ0gc_h
#define τπ0gc_h


#include <algorithm>


#include "debug.hh"
#include "utf9.hh"
#include "pi0-types.hh"

#include "begin.hh"

namespace τ
{


struct π0F  // local frame
{
  V<π0r> xs;
  π0F(uN vs) { xs.assign(vs, π0hω); }
};


struct π0o9r  // heap-local reference
{
  uN a;       // offset within heap
  uN size ()     const { return sizeof(uN) + u9sb(u9sq(sizeof(uN))); }
  uN write(ζp m) const { W<uN>(m, u9ws(m, 0, u9t::pi, sizeof(uN), true), a); return 0; }
};


struct π0o9c  // complex-value rewrite
{
  sletc ds = u9tm{u9t::tuple, u9t::map, u9t::set};

  π0r     x;  // old-heap address of thing being rewritten
  B      &h;  // old heap used to resolve pointers
  S<π0r> &m;  // set of objects with multiple references (input, no-inline)
  S<π0r> &w;  // set of references that need to be rewritten (output, new-heap addresses)

  V<P<π0r, iN>> mutable o;      // size shifts in the form (index, ∑delta)
  uN            mutable s = 0;  // == x.osize() + o[-1].second

  π0o9c(π0r x_, B &h_, S<π0r> &m_, S<π0r> &w_) : x(x_), h(h_), m(m_), w(w_) {}

  i9 operator[](π0r a) const { return i9{h.data() + a}; }
  bool       in(π0r a) const { return (*this)[a].type() == u9t::pi && !m.contains(a); }

  iN disp(π0r a) const
    { uN l = 0, u = o.size();
      while (l + 1 < u)
      { let m = l + u >> 1;
        (a < o[m].first ? u : l) = m; }
      return o[l].second; }

  uN size() const
    { if (s) return s;

      // We know exactly which references we want to inline, so the first
      // step is to walk through and allocate size for them. We'll mark
      // each one into the `o` vector.
      //
      // `o` also contains splices that result from container sizes requiring
      // different bit-sizes to encode.
      let           i = (*this)[x];
      Sk<π0r>       c;   // current containers
      V<P<π0r, iN>> ss;  // size splices
      V<P<π0r, iN>> rs;  // rewrite splices

      iN d = 0;          // total displacement so far
      o.clear();

      for (π0r j = 0; j <= i.osize(); j += (*this)[x + j].osize())
      { each:
        for (; !c.empty() && j >= c.top() + (*this)[c.top()].osize(); c.pop())
        { let sn = u9sq(j - c.top() + d);     // new size encoding
          let so = (*this)[c.top()].stype();  // original size encoding
          let ds = u9sb(sn) - u9sb(so);       // size-encoding difference
          d += ds;
          if (ds) ss.push_back(mp(c.top(), ds)); }

        if (j >= i.osize()) break;

        let z = (*this)[x + j];
        let t = z.type();
        if (z.flagged())
          if (ds[t])                { c.push(j); j += u9sb(z.stype()); goto each; }
          else if (t == u9t::index) TODO("π₀o9c index");
          else if (in(x + j))
          { let ds = (*this)[z.π()].osize() - z.osize();
            d += ds;
            rs.push_back(mp(j, ds)); }}

      // Now merge our splices into the main compiled-splice list. Note that
      // container size splices are out of order, whereas rewrite splices
      // are not.
      std::sort(ss.begin(), ss.end(), [](auto a, auto b) { return a.first < b.first; });

      uN j = 0, k = 0;
      for (d = 0; j < ss.size() && k < rs.size();)
      { let &[si, sd] = ss[j];
        let &[ri, rd] = rs[k];
        if      (si < ri) o.push_back(mp(si, d += sd     )), ++j;
        else if (ri < si) o.push_back(mp(ri, d +=      rd)),      ++k;
        else              o.push_back(mp(si, d += sd + rd)), ++j, ++k; }
      while (j < ss.size()) o.push_back(mp(ss[j].first, d += ss[j].second));
      while (k < rs.size()) o.push_back(mp(rs[k].first, d += rs[k].second));
      return s = i.osize() + d; }

  uN write(ζp m) const
    { TODO("π0o9c write"); }
};

template<> struct o9_<π0o9r> { sletc v = true; };
template<> struct o9_<π0o9c> { sletc v = true; };


struct π0h
{
  B      h;
  V<π0r> d;   // data stack
  V<π0F> f;   // stack of local frames
  V<π0r> p;   // pinned values
  f64    lh;  // live set → heap size factor
  uN     is;  // inlining size: i9s smaller than this are inlined immediately

  π0h(f64 lh_ = 4.0,
      uN  is_ = 64,
      uf8 hb_ = ζb0)
    : lh{lh_}, is{is_}
    { h.reserve(1ul << hb_);
      p.reserve(64); }

  template<o9n_ T>
  π0r operator<<(T const &x)
    { return *this << o9(x); }

  template<o9__ T>
  π0r operator<<(T const &o)
    { let s = o.size();
      if (h.size() + s > h.capacity()) gc(s);
      return h << o; }

  π0r operator<<(i9 x)
    { return x.a >= h.data() && x.a < h.data() + h.size() && x.osize() >= is
           ? *this << π0o9r{x}
           : *this << o9(x); }


  uN size_of     (π0r i) { return (*this)[i].osize(); }
  i9 operator[]  (π0r i) { if (τπ0debug_bounds_checks) A(i < h.size(), "π₀[π₀r]: " << i << " ≥ " << h.size()); return i9{h.data() + i}; }
  uN pin         (π0r a) { p.push_back(a); return p.size() - 1; }
  i9 pinned      (uN i)  {                 return (*this)[τπ0debug_bounds_checks ? p.at(i) : p[i]]; }
  π0h &clear_pins()      { p.clear();      return *this; }

  void trace(S<π0r> &r, S<π0r> &m, π0r i)
    { if (!r.insert(i).second) m.insert(i);
      else
      { let a = (*this)[i];
        if (a.flagged() && u9tm{u9t::index, u9t::tuple, u9t::map, u9t::set}[a.type()])
          for (let x : a) trace(r, m, x.a - h.data()); }}

  void live(S<π0r> &r, S<π0r> &m)
    { for (let  x : d)                    trace(r, m, x);
      for (let &v : f) for (let x : v.xs) trace(r, m, x);
      for (let  x : p)                    trace(r, m, x); }

  void gc(uN s)  // GC with room for live set + s
    { S<π0r> rs, ms;  live(rs, ms);
      uN     ls = s;  for (let r : rs) ls += size_of(r);
      B      h_;      h_.reserve(Sc<uN>(ls * lh));

      M<π0r, π0r> ns;  // old → new heap address map
      S<π0r>      rw;  // set of references in new heap that need to be updated

      // No object with multiple references will be inlined; we already inline
      // anything under some fixed size, so at this point we want to save space.
      for (let m : ms) if (m != π0hω) ns[m] = h_ << o9((*this)[m]);

      // Everything else can and should be inlined into whoever refers to it.
      for (let r : rs)
        if (r != π0hω && !ms.contains(r))
        { let i = (*this)[r];
          ns[r] = i.flagged() ? h_ << π0o9c(r, h, ms, rw)
                              : h_ << o9(i); }
      ns[π0hω] = π0hω;

      for (auto &x : d)                      x = ns[x];
      for (auto &x : f) for (auto &y : x.xs) y = ns[y];
      for (auto &x : p)                      x = ns[x];
      h.swap(h_); }


#if τπ0debug_bounds_checks
  i9 di(uN i)            { return (*this)[d.at(d.size() - 1 - i)]; }
  i9 fi(uN i, uN fi = 0) { return (*this)[f.at(f.size() - 1 - fi).xs.at(i)]; }

  template<class T>
  π0h &fs(uN i, T const &x) { f.at(f.size() - 1).xs.at(i) = *this << x; return *this; }
#else
  i9 di(uN i)            { return (*this)[d[d.size() - 1 - i]]; }
  i9 fi(uN i, uN fi = 0) { return (*this)[f[f.size() - 1 - fi].xs[i]]; }

  template<class T>
  π0h &fs(uN i, T const &x) { f[f.size() - 1].xs[i] = *this << x; return *this; }
#endif

  template<class T>
  π0h &dpush(T const &x) { d.push_back(*this << x); return *this; }
  π0h &dpop(uN n = 1)    { d.resize(d.size() - n);  return *this; }

  π0h &fpush(uN vs)      { f.push_back(π0F(vs));    return *this; }
  π0h &fpop()            { f.pop_back();            return *this; }
};


}

#include "end.hh"


#endif
