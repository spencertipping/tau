#ifndef τπ0_gc_markset_h
#define τπ0_gc_markset_h


#include "debug.hh"
#include "types.hh"
#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc-heap.hh"
#include "pi0-gc-heapspace.hh"

#include "begin.hh"

namespace τ
{


struct π0ms
{
  π0h          &h;
  M<π0r, π0r>   n;  // new address for each reference
  Mo<π0r, o9V*> m;  // writer for each marked reference

  bool contains(π0r x) const { return n.contains(x); }
  π0r  at      (π0r x) const { return n.at(x); }

  void gc() { TODO("gc"); }

  void mark(π0r x)
    { if (n.contains(x)) return;


    }
};


}

#include "end.hh"


#endif
