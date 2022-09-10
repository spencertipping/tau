#ifndef tau_φ9_h
#define tau_φ9_h


#include "types.hh"
#include "phi.hh"
#include "Phi.hh"


#include "begin.hh"

namespace tau
{


struct φ9
{
  template<class T>
  struct φo
  {
    Va<T, o9f<u9_heapref>> x;

    uN size ()     const { return x.index() ? std::get<1>(x).size()   : std::get<0>(x).size(); }
    uN write(ζp m) const { return x.index() ? std::get<1>(x).write(m) : std::get<0>(x).write(m); }
  };


  u64 im  = 0;  // total inbound messages
  u64 om  = 0;  // total outbound messages
  u64 ohc = 0;  // messages over half-ζ-capacity
  u64 ov  = 0;  // messages that overflowed (and were boxed)

  // Most recently-received heap-allocated message, which remains live
  // until (1) it is taken (then you free it manually), (2) you read
  // another message, (3) you destroy this φ9, or (4) you explicitly
  // call .free().
  ζp b = nullptr;

  ~φ9() { free(); }


  ic bool os(uN s, uN zs)
    { ++om;
      if (s >= zs >> 1) ++ohc;
      let v = s >= zs - 1;
      if (v) ++ov;
      return v; }


  i9   take() { let r = b; b = nullptr; return r; }
  void free() { if (b) { std::free(b); b = nullptr; } }

  template<class R>
  i9 r(i9 x, ζ<R>&)
    { ++im;
      free();
      if (x.type() == u9t::heapref) b = x = *x;
      return x; }

  template<class W, class Z> ic auto w(W const &x, ζ<Z> &z)
    { if constexpr (o9_<W>::v) { return os(x.size(), z.b.c) ? φo<W>          {o9box(x)} : φo<W>          {x}; }
      else      { let o = o9(x); return os(o.size(), z.b.c) ? φo<decltype(o)>{o9box(o)} : φo<decltype(o)>{o}; } }
};


}

#include "end.hh"


#endif
