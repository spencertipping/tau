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
  π0r     x;  // thing being rewritten
  B      &h;  // heap used to resolve pointers
  S<π0r> &m;  // set of objects with multiple references
  f64     lh; // live set → heap size expansion factor
  uN      is; // inlining size for this rewrite

  uN mutable s = 0;

  π0o9c(π0r x_, B &h_, S<π0r> &m_, f64 lh_, uN is_) : x(x_), h(h_), m(m_), lh(lh_), is(is_) {}
  π0o9c(π0r x_, B &h_, S<π0r> &m_, f64 lh_)         : x(x_), h(h_), m(m_), lh(lh_)
    {

    }

  i9 operator[](π0r a) const { return i9{h.data() + a}; }
  bool       in(π0r a) const { return !m.contains(a) || (*this)[(*this)[a].π()].osize() <= is; }

  uN size()  const { if (!s) s = isize(); return s + u9sb(u9sq(s)); }
  uN isize() const
    { // TODO
    }

  uN write() const
    {}
};


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

  template<class T>
  π0r operator<<(T const &x)
    { let o = o9(x);
      let s = o.size();
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

      // TODO: inline single refs into references
      // TODO: write multiple-refs first

      // TODO: deduplicate slices
      // TODO: rewrite complex values here
      M<π0r, π0r> ns;
      for (let r : rs) if (r != π0hω) ns[r] = h_ << o9((*this)[r]);
      ns[π0hω] = π0hω;

      for (auto &x : d) x = ns[x];
      for (auto &x : f) for (auto &y : x.xs) y = ns[y];
      for (auto &x : p) x = ns[x];
      h.swap(h_); }


#if τπ0debug_bounds_checks
  i9 di(uN i)            { return (*this)[d.at(d.size() - 1 - i)]; }
  i9 fi(uN i, uN fi = 0) { return (*this)[f.at(f.size() - 1 - fi).xs.at(i)]; }

  template<class T>
  π0h &fs(uN i, T const &x) { f.at(f.size() - 1).xs.at(i) = *this << o9(x); return *this; }
#else
  i9 di(uN i)            { return (*this)[d[d.size() - 1 - i]]; }
  i9 fi(uN i, uN fi = 0) { return (*this)[f[f.size() - 1 - fi].xs[i]]; }

  template<class T>
  π0h &fs(uN i, T const &x) { f[f.size() - 1].xs[i] = *this << o9(x); return *this; }
#endif

  template<class T>
  π0h &dpush(T const &x) { d.push_back(*this << o9(x)); return *this; }
  π0h &dpop(uN n = 1)    { d.resize(d.size() - n);      return *this; }

  π0h &fpush(uN vs)      { f.push_back(π0F(vs));        return *this; }
  π0h &fpop()            { f.pop_back();                return *this; }
};


}

#include "end.hh"


#endif
