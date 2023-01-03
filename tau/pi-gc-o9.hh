#ifndef τπgc_o9_h
#define τπgc_o9_h


#include "utf9.hh"
#include "types.hh"
#include "pi-types.hh"
#include "pi-gc-heap.hh"

#include "begin.hh"

namespace τ
{


struct πo9f : virtual o9V  // flatten heap object for export
{
  i9                    r;
  M<πr, πo9f*> mutable c;
  uN            mutable s = 0;

  static i9 f(i9 x) { while (x.is_πref()) x = *x; return x; }

  πo9f(i9 r_) : r(f(r_)) {}
  ~πo9f() { for (let &[_, v] : c) delete v; }

  πo9f &operator=(πo9f &&x)
    { r = x.r;
      c = std::move(x.c);
      s = x.s;
      return *this; }

  uN size()  const { return isize() + u9sb(u9sq(isize())); }
  uN isize() const
    { if (s)            return s;
      if (!r.flagged()) return s = r.osize();

      // We must be a flagged collection here. If we weren't, we'd have
      // to be a reference, which we can't be due to f() in the ctor.
      for (let x : r)
        if (!x.flagged()) s += x.osize();
        else              s += (c[x.a] = new πo9f(x))->size();
      return s; }

  uN write(ζp m) const
    { if (!r.flagged()) return o9i9{r}.write(m);
      uN o = u9ws(m, 0, r.type(), isize(), false);
      for (let x : r)
        if (!x.flagged())
        { let y = o9i9{x};
          y.write(m + o); o += y.size(); }
        else
        { let y = c.at(x.a);
          y->write(m + o); o += y->size(); }
      return 0; }
};


struct πo9q : virtual o9V  // quote heap object in bytes
{
  πo9f o;
  πo9q(i9 r_) : o(r_) {}

  uN size() const { return o.size() + u9sb(u9sq(o.size())); }
  uN write(ζp m) const
    { let i = u9ws(m, 0, u9t::bytes, o.size(), false);
      o.write(m + i);
      return 0; }
};


struct πho9 : virtual o9V  // write heap object for GC
{
  πh        &h;
  i9    const r;      // oldspace object
  πr mutable o;      // owner, if any (i.e. which i9 is inlining us)
  πr mutable n = 0;  // newspace location
  uN  mutable s = 0;  // cached newspace inner size

  πho9(πh &h_, i9 r_, πr o_) : h(h_), r(r_), o(o_) {}


  // Always inline objects if they are smaller than the heap's inlining
  // size threshold. This improves locality and can save space, since
  // some objects are smaller than a reference that would point to them.
  //
  // Note that roots can be inlined anywhere, at which point they become
  // owned by their inlined location.
  bool inline_at(πr x) const
    { if (!o) o = x;
      return r.osize() <= h.is || o == x; }


  uN size()  const { return isize() + u9sb(u9sq(isize())); }
  uN isize() const
    { if (s) return s;

      // Simple case: we're not a collection, so we can either copy i9
      // or, if we're a reference, delegate to our referent (assuming
      // that refereent hasn't yet been claimed).
      if (!u9coll[r.type()])
      { if (!r.is_πref()) return s = r.size();
        let t = h.claim(r.deref(), r.a);
        return s = (t && t->inline_at(r.a) ? t->size() : r.size()); }
      else
      { // We're a collection, so we need to try to claim every marked
        // object. If there are no marked objects in our content range,
        // then we can vectorized-copy the original.
        let i = h.cb(r);
        let e = h.ce();

        // Simple case: no references or marked objects, so we can copy
        // the original i9 verbatim.
        if (!r.flagged() && (i == e || (*i).first >= r.next()))
          return s = r.size();

        // Complex case: claim each child. If we can't claim a child,
        // then we'll write in a reference in its place. Three things can
        // happen here:
        //
        // 1. o == nullptr, in which case the object wasn't marked, so
        //    it gets copied verbatim.
        // 2. o is claimed by us, in which case we inline it.
        // 3. o is claimed by someone else, in which case we write a
        //    reference to it -- whether or not it was a reference to
        //    start with.
        //
        // NOTE: this can be optimized by walking `i` forwards and using
        // pointer differences to infer the size of other elements. This
        // results in fewer map lookups.
        for (let x : r)
        { let c = x.deref();  // remove all reference layers
          let o = h.claim(c, x.a);
          s += !o ? c.osize()
                  : o->inline_at(x.a) ? o->size() : πo9r(c).size(); }
        return s; } }


  uN write(ζp m) const
    { n = m;
      if (!u9coll[r.type()])
      { if (!r.is_πref()) return o9i9{r}.write(m);
        let t = h.claim(r.deref(), r.a);
        return t && t->inline_at(r.a) ? t->write(m) : o9i9{r}.write(m); }
      else
      { let i = h.cb(r);
        let e = h.ce();
        if (!r.flagged() && (i == e || (*i).first >= r.next()))
          return o9i9{r}.write(m);

        // Reconstruct the container by writing children one by one.
        //
        // TODO: optimize by vectorizing multiple adjacent unmarked
        // children when possible, minimizing the number of claims
        bool f = false;
        uN   x = u9ws(m, 0, r.type(), isize(), false);
        for (let y : r)
        { let c  = y.deref();
          let o  = h.claim(c, y.a);
          let x0 = x;
          if      (!o)                { let w = o9i9{c}; A(!w .write(m + x), "GC internal error"); x += w .size(); }
          else if (o->inline_at(y.a)) {                  A(!o->write(m + x), "GC internal error"); x += o->size(); }
          else
          { A(o->n, "π₀o9 ∅ref");
            let w = πo9r(o->n);
            A(!w.write(m + x), "GC internal error"); x += w.size(); }
          f = f || u9ts_f(R<u8>(m, x0)); }
        if (f) m[0] |= u9f;
        return 0; } }
};


}

#include "end.hh"


#endif
