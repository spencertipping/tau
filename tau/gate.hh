#ifndef τgate_h
#define τgate_h


#include "ctypes.hh"
#include "types.hh"
#include "Lambda.hh"

#include "begin.hh"

namespace τ
{


// Λ wake-gate: manage one or more λs that are blocked on some condition,
// wake all of them when w() is called (and send a message).
//
// This class holds a reference to a gate container, which outlasts λg so
// that all awoken λs will have access to messages being sent to them.
template<class T>
struct λg;


// Gate container: this contains all of the logic required to send messages
// and wake λs that are blocked.
template<class T>
struct λgc
{
  λgc(Λ &l_) : l(l_) {}

  // NOTE: by-value Sp<> acquisition here is necessary to ensure λgc lives
  // through this whole function call
  T y(λs s, Sp<λgc<T>> p)
    { ps.push_back(l.i());
      l.y(s);
      return m; }

  void w(T x)
    { m = x;
      for (let p : ps) l.r(p);
      ps.clear(); }

protected:
  Λ     &l;
  V<λi>  ps;
  T      m;
};


template<class T>
struct λg
{
  λg(Λ &l) : c(λgc<T>(l)) {}

  // Yield until awoken, return the message sent when waking; note that
  // the shared ptr holds λgc in memory until after the return, guaranteeing
  // that it can safely refer to its message long enough to copy it back to
  // us.
  T y(λs s) { return c->y(s, c); }

  // Wake all blocked λs, returning x from their λg::y() calls.
  //
  // NOTE: you can awaken multiple times before the λs are scheduled. If you
  // do, the last w() argument is used as the wake message and previous values
  // are discarded.
  void w(T x)  { c->w(x); }

protected:
  Sp<λgc<T>> c;
};


}

#include "end.hh"


#endif
