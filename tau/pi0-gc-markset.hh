#ifndef τπ0_gc_markset_h
#define τπ0_gc_markset_h


#include "debug.hh"
#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"
#include "pi0-gc-heapspace.hh"
#include "pi0-gc-o9.hh"

#include "begin.hh"

namespace τ
{


struct π0ms
{
  π0h            &h;
  uNc             mg;  // maximum generation being collected
  Mo<π0r, π0ho9*> m;   // writer for each marked reference
  S<π0r>          t;   // set of true roots

  π0ms(π0h &h_, uN mg_, uN an) : h(h_), mg(mg_) {}

  ~π0ms() { for (let &[_, o] : m) delete o; }


  bool contains(π0r x) const { return m.contains(x) && m.at(x); }
  π0r  at      (π0r x) const { return m.at(x)->n; }


  // Mark x as having an external reference -- i.e. being a true root.
  // This is a no-op if x is already marked (as either type of root),
  // so we won't incorrectly truthify an already false root here.
  //
  // NOTE: external references, unlike internal ones, are not strictly
  // transparent. As a result, we must mark the exact referent, even if
  // it is itself a reference that points elsewhere. This guarantees
  // that external reference locality is preserved.
  //
  // NOTE: off-heap memory is compacted into newgen.
  void me(i9 x)
    { if (m.contains(x)) return;
      let hg = h.gen(x);
      if (hg != π0h::gω && hg > mg) return;
      m[x] = nullptr;
      t.insert(x);
      if (x.is_πref()) mi(x.deref());
      else
        for (let y : x.flags()) if (y.is_πref()) mi(y.deref()); }

  // Mark x as having an internal reference -- i.e. being a false root.
  void mi(i9 x) { t.erase(x); me(x); }


  π0ho9 *ho9(π0r f, π0r o) { return new π0ho9(h, f, o); }


  // Attempt to claim f as an inline of o, returning one of two things:
  //
  // + nullptr, if f isn't marked (i.e. nothing refers to it, but it's
  //   a live object because it's contained by a live object)
  // + f's o9, if f is marked; the o9 may or may not define its owner
  //   to be the claimant, so the claimant should check this
  //
  // Note that the nullptr case means f is an uncontended child; that
  // is, it has exactly one liveness-link from its container and is
  // guaranteed to be inlinable by that container. We return nullptr
  // instead of allocating an o9 to save space.
  π0ho9 *claim(π0r f, π0r o)
    { return !m.contains(f)
           ?  nullptr
           :  m[f] ? m[f] : (m[f] = ho9(f, o)); }

  // FIXME: unordered map + sorted vector
  auto cb(π0r x) const { return m.lower_bound(x); }
  auto ce()      const { return m.end(); }


  uN plan()
    { // me() and mi() have so far conspired to identify true roots
      // from a linkage perspective, but not containment. That is,
      // suppose we have a=(b c d) and both `a` and `c` are external
      // roots. We haven't yet identified `c` to be a false root of
      // `a`. We do this by iterating through the root set in order
      // and removing any that are properly contained by others.
      V<π0r> rs; rs.reserve(m.size());
      for (let &[k, _] : m) rs.push_back(k);
      //std::sort(rs.begin(), rs.end());  // FIXME: already sorted
      for (uN i = 0; i < rs.size(); ++i)
        for (i9 x = rs[i++], e = x.next().a;
             i < rs.size() && rs[i] < e;
             ++i)
          t.erase(rs[i]);

      // Now calculate the new live-set size, which involves deciding
      // which container gets to inline each object. This is handled
      // by claim(), cb(), and ce().
      //
      // First, m[] has an entry for every claimable value. This matters
      // because each o9 consults it to (1) claim its contained things,
      // and (2) figure out whether its contained things have been
      // claimed by someone else. Missing values mean that nobody refers
      // to the object, so it may be live-by-containment but it can be
      // copied verbatim.
      //
      // TODO: sort true roots by generation; we must always create
      // oldgen pointers before newgen ones
      uN s = 0;
      for (let x : t) s += (m[x] = ho9(x, nullptr))->size();
      return s; }


  // TODO: add generational control to this function; we should break
  // early if we're tenuring some objects to older generations.
  π0ms &operator>>(B &h_)
    { // Copy true roots into new heap in the same order we used
      // when calculating sizes.
      for (let x : t) h_ << o9vd{m[x]};
      return *this; }
};


}

#include "end.hh"


#endif
