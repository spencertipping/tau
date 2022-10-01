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


// FIXME: π0h can't return i9s tied to the heap because computation
// can potentially move them with GC. Instead, we need to return
// pin references that will be updated automatically if GC happens;
// the caller must dereference them before every use.
//
// This will also solve the auto-slicing problem, which right now
// involves copying values around.
//
// NOTE: we probably won't ever have more than a few values in play
// at a time; we don't hang onto heap values much. So the pin set can
// be a small LIFO vector that is cleared frequently.

struct π0h
{
  B      h;
  V<π0r> d;  // data stack
  V<π0F> f;  // stack of local frames
  S<π0r> p;  // pinned values

  π0h(uf8 hb = ζb0) { h.reserve(1ul << hb); }

  template<o9__ T>
  π0r operator<<(T const &x)
    { let o = o9(x);
      let s = o.size();
      if (h.size() + s > h.capacity()) gc(s);
      return h << o; }


  uN size_of   (π0r i) { return (*this)[i].osize(); }
  i9 operator[](π0r i) { return (*this)(i9{h.data() + i}); }

  // TODO: dereference lazy slices
  i9 operator()(i9 i)  { return i; }

  void live(S<π0r> &r) const
    { for (let x : d) r.insert(x);
      for (let &x : f) for (let y : x.xs) r.insert(y); }

  void gc(uN s)  // GC with room for live set + s
    { S<π0r> rs;                live(rs);
      uN     ls = s;            for (let r : rs) ls += size_of(r);
      uN     c  = h.capacity(); while (c >> 1 < ls) c <<= 1;
      B      h_;                h_.reserve(c);

      // TODO: deduplicate slices
      M<π0r, π0r> ns;
      for (let r : rs)
      { if (r == π0hω) continue;
        ns[r] = h_.size(); h_ << o9((*this)[r]); }
      ns[π0hω] = π0hω;

      for (auto &x : d) x = ns[x];
      for (auto &x : f) for (auto &y : x.xs) y = ns[y];
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
