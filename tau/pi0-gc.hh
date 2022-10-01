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


struct π0h
{
  B      h;
  V<π0r> d;   // data stack
  V<π0F> f;   // stack of local frames
  V<π0r> p;   // pinned values
  uf8    lb;  // live-set bits

  π0h(uf8 lb_ = 2,
      uf8 hb_ = ζb0)
    : lb{lb_}
    { h.reserve(1ul << hb_);
      p.reserve(64); }

  template<o9__ T>
  π0r operator<<(T const &x)
    { let o = o9(x);
      let s = o.size();
      if (h.size() + s > h.capacity()) gc(s);
      return h << o; }


  uN size_of   (π0r i) { return (*this)[i].osize(); }
  i9 operator[](π0r i) { return i9{h.data() + i}; }

  void trace(S<π0r> &r, S<π0r> &m, π0r i)
    { if (!r.insert(i).second) m.insert(i);
      else
      { let a = (*this)[i];
        if (a.flagged() && u9tm{u9t::tuple, u9t::map, u9t::set}[a.type()])
          for (let x : a) trace(r, m, x.a - h.data()); }}

  void live(S<π0r> &r, S<π0r> &m)
    { for (let  x : d)                    trace(r, m, x);
      for (let &v : f) for (let x : v.xs) trace(r, m, x);
      for (let  x : p)                    trace(r, m, x); }

  void gc(uN s)  // GC with room for live set + s
    { S<π0r> rs, ms;            live(rs, ms);
      uN     ls = s;            for (let r : rs) ls += size_of(r);
      uN     c  = h.capacity(); while (c >> 1 < ls) c <<= 1;
      B      h_;                h_.reserve(c);

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
