#ifndef τπ0gc_o9_h
#define τπ0gc_o9_h


#include "utf9-types.hh"
#include "utf9.hh"
#include "types.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"

#include "begin.hh"

namespace τ
{


struct π0ho9 : virtual o9V
{
  π0h               &h;
  i9           const r;      // oldspace object
  π0ho9 const *const o;      // owner, if any (i.e. who is inlining us)
  π0r        mutable n = 0;  // newspace location
  uN         mutable s = 0;  // cached newspace inner size

  π0ho9(π0h &h_, i9 r_, π0ho9 const *o_) : h(h_), r(r_), o(o_) {}


  uN size()  const { return isize() + u9sb(u9sq(isize())); }
  uN isize() const
    { if (s) return s;

      // Simple case: we're not a collection, so we can either copy i9
      // or, if we're a reference, delegate to our referent (assuming
      // that refereent hasn't yet been claimed).
      if (!u9coll[r.type()])
      { let t = h.claim(h(r), this);
        return s = (t && t->o == this ? t->size() : r.size()); }
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
        { let c = h(x);  // remove all reference layers
          let o = h.claim(c, this);
          s += !o ? c.osize()
                  : o->o == this ? o->size() : π0o9r(c).size(); }
        return s; } }


  uN write(ζp m) const
    { n = m;
      if (!u9coll[r.type()])
      { let t = h.claim(h(r), this);
        return t && t->o == this ? t->write(m) : o9i9{r}.write(m); }
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
        { let c  = h(y);
          let o  = h.claim(c, this);
          let x0 = x;
          if (!o)                { let w = o9i9{c};     A(!w .write(m + x), "GC internal error"); x += w .size(); }
          else if (o->o == this) {                      A(!o->write(m + x), "GC internal error"); x += o->size(); }
          else                   { let w = π0o9r(o->n); A(!w .write(m + x), "GC internal error"); x += w .size(); }
          f = f || u9ts_f(R<u8>(m, x0)); }
        if (f) m[0] |= u9f;
        return 0; } }
};


}

#include "end.hh"


#endif
