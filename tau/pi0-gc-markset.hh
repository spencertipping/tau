#ifndef τπ0_gc_markset_h
#define τπ0_gc_markset_h


#include <algorithm>
#include <cstring>


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
  π0h           &h;
  M<π0r, π0r>    n;  // new address for each reference
  M<π0r, π0ho9*> m;  // writer for each marked reference

  bool contains(π0r x) const { TODO("contains"); }
  π0r  at      (π0r x) const { TODO("at"); }

  void gc() { TODO("gc"); }

  void me(π0r x) { TODO("me"); }
};


}

#include "end.hh"


#endif
