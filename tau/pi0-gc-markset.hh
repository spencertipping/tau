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
  S<π0r>        f;   // set of false references
  M<π0r, o9V*>  m;   // writer for each marked reference

  ~π0ms() { for (let &[_, o] : m) delete o; }


  bool contains(π0r x) const { return r.contains(x); }
  π0r  at      (π0r x) const { return r.at(x); }


  void mark(i9 x)
    { if (m.contains(x) || h.gen(x) > mg) return;
      m[x] = nullptr;
      r[x] = nullptr;
      for (let y : x.flags()) if (y.is_πref()) mark(*y); }

  void move(π0r f, π0r t)
    { A(!r.contains(f),
        "doubly relocating " << f << " to " << r[f] << " and " << t);
      r[f] = t; }


  uN plan()
    { for (auto i = r.begin(); i != r.end(); ++i)
        for (i9 x = (*i++).first, e = x.next();
             i != r.end() && (*i).first < e;
             ++i)
          f.insert((*i).first);
      uN s = 0;
      for (auto &[x, o] : m)
        if (!f.contains(x))
          s += (o = new π0ho9{x, &h})->size();
      return s; }
};


}

#include "end.hh"


#endif
