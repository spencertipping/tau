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
  π0h          &h;
  uNc           mg;  // maximum generation being collected
  Mo<π0r, π0r>  r;   // relocation table
  S<π0r>        t;   // set of true roots
  M<π0r, o9V*>  m;   // writer for each marked reference

  ~π0ms() { for (let &[_, o] : m) delete o; }


  bool contains(π0r x) const { return r.contains(x); }
  π0r  at      (π0r x) const { return r.at(x); }


  // Mark x as having an external reference -- i.e. being a true root.
  void me(i9 x)
    { if (m.contains(x) || h.gen(x) > mg) return;
      m[x] = nullptr;
      t.insert(x);
      for (let y : x.flags()) if (y.is_πref()) mi(*y); }

  // Mark x as having an internal reference -- i.e. being a false root.
  void mi(i9 x) { t.erase(x); me(x); }


  void claim(π0r f, π0ho9 *o) { r[f] = Rc<π0r>(o); }
  void move (π0r f, π0r t)    { r[f] = t; }


  π0ms &operator>>(B &h_)
    { // Copy true roots into new heap in the same order we used
      // when calculating sizes.


    }


  uN plan()
    { // me() and mi() have so far conspired to identify true roots
      // from a linkage perspective, but not containment. That is,
      // suppose we have a=(b c d) and both `a` and `c` are external
      // roots. We haven't yet identified `c` to be a false root of
      // `a`. We do this by iterating through the root set in order
      // and removing any that are properly contained by others.
      V<π0r> rs; rs.reserve(m.size());
      for (let &[k, _] : m) rs.push_back(k);
      std::sort(rs.begin(), rs.end());
      for (auto i = rs.begin(); i != rs.end(); ++i)
        for (i9 x = *i++, e = x.next();
             i != rs.end() && *i < e;
             ++i)
          t.erase(*i);

      // Now calculate the new live-set size, which involves deciding
      // which container gets to inline each object. π₀ho9 claims an
      // object by setting r[x] = this, simultaneously indicating that
      // the object has a location and identifying which one it is.
      //
      // r[] must be ordered because o9 issues range queries to detect
      // claimed children within containers -- this is more efficient
      // than checking each child point-wise.
      //
      // If an o9 needs to create more o9s to track children, it can
      // store additional entries in m[], which should already have
      // enough space.
      uN s = 0;
      for (let x : t) s += (m[x] = new π0ho9{x, &h})->size();
      return s; }
};


}

#include "end.hh"


#endif
