#ifndef τπ0gc_h
#define τπ0gc_h


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
  S<π0r> &w;  // set of references that need to be rewritten (output)

  V<P<π0r, iN>> mutable o;      // size shifts in the form (index, ∑delta)
  uN            mutable s = 0;  // == x.osize() + o[-1].second

  π0o9c(π0r x_, B &h_, S<π0r> &m_, S<π0r> &w_) : x(x_), h(h_), m(m_), w(w_) {}

  i9 operator[](π0r a) const { return i9{h.data() + a}; }
  bool       in(π0r a) const { return (*this)[a].type() == u9t::pi && !m.contains(a); }

  uN size()  const { if (!s) s = isize(); return s + u9sb(u9sq(s)); }
  uN isize() const
    { // We know exactly which references we want to inline, so the first
      // step is to walk through and allocate size for them. We'll mark
      // each one into the `o` vector.
      let i = (*this)[x];
      V<π0r>        c;  // flagged container offsets
      V<P<π0r, iN>> d;  // size deltas from rewrites
      V<P<π0r, iN>> e;  // size deltas from container size encodings

      // TODO: modify this loop so we track all containers while we're going
      for (π0r j = 0; j < i.osize(); j += (*this)[x + j].osize())
      { let z = (*this)[x + j];
        if      (in(x + j))                   d.push_back(mp(j, (*this)[z.π()].osize() - z.osize()));
        else if (z.type() == u9t::pi)         w.insert(x + j);
        else if (z.flagged() && ds[z.type()]) j += u9sb(z.stype()), c.push_back(j);
        else if (z.flagged()
                 && z.type() == u9t::index)   TODO("π₀o9c index"); }

      uN t = i.size();  // NOTE: inner size, since we're isize()
      for (let &[_, d] : o) t += d;
      return t; }

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
    { return x.a >= h.data() && x.a < h.data() + h.size() && x.osize() > is
           ? *this << π0o9r{x}
           : *this << o9(x); }


  uN size_of   (π0r i) { return (*this)[i].osize(); }
  i9 operator[](π0r i) { return i9{h.data() + i}; }

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

      // Everything else can and should be inlined into whoever contains it.
      for (let r : rs)
        if (r != π0hω && !ms.contains(r))
        { let i = (*this)[r];
          ns[r] = i.flagged() ? h_ << π0o9c(r, h, ms, rw)
                              : h_ << o9(i); }
      ns[π0hω] = π0hω;

      for (auto &x : d) x = ns[x];
      for (auto &x : f) for (auto &y : x.xs) y = ns[y];
      for (auto &x : p) x = ns[x];
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
