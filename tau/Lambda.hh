#ifndef τΛ_h
#define τΛ_h


#include "arch.hh"
#include "debug.hh"
#include "types.hh"
#include "shd.hh"

#include "lambda.hh"


#include "begin.hh"

namespace τ
{


// Exception used to kill λs
struct Λx_ final {};
sletc λx_ = Λx_{};

// Instantly destroy whichever λ calls this function
inline void λx() { throw λx_; }


// A single Λ task, which is a λ plus some metadata
struct Λt final
{
  λ  l;
  λs s;

  Λt(λf &&f) : l(λ(mo(f))), s(λs::S) {}

  void run()            {        l(); }
  bool done()     const { return l.done(); }
  bool runnable() const { return s == λs::R || s == λs::X; }
};


struct Λ;

Λ   &Λ_();    // current Λ (the one running the current thread)
void Λ_(Λ*);  // set current Λ


// NOTE: managed λs should yield out with Λ.y, not λy() defined by lambda.hh
struct Λ final
{
  Λ(Λ&)  = delete;
  Λ(Λ&&) = delete;
  Λ() { csws.reserve(8); }

  // This deserves some explanation. ~Λ causes ~λ, which causes ~λf, which
  // causes ~ξ, which causes λg::w(), which attempts to wake more λs.
  //
  // Well, by that point our data structures are in some undefined state
  // of deallocation; it's bad. So we set fin here to indicate that the
  // world is ending, nothing matters from here.
  ~Λ() { fin = true; Λ_(nullptr); }

  bool e(λi i) const { return fin ? false : ls.contains(i); }
  λi   i()     const { return ri; }
  u64  cs()    const { return cs_; }

  void clear();

  λi  c(λf &&);
  Λ  &x(λi);
  Λ  &y(λs);               // yield currently-running λ with specified yield state
  Λ  &r(λi, λs = λs::Y);   // set λi to runnable, or other state

  Λ  &operator<<(λi);      // run λi for one quantum
  λi  operator()() const;  // find next λi to run
  Λ        &step();        // run all runnable λs for one quantum

  uN n() const { return ls.size(); }

  Λ &csw(F<void()> &&f) { csws.push_back(mo(f)); return *this; }


protected:
  M<λi, Sp<Λt>> ls;           // all λs
  S<λi>         rs;           // run set
  V<F<void()>>  csws;         // context switch callbacks
  ΣΘΔ           qΘ;           // quantum time measurement
  λi            ni  = 0;      // next λi (always nonzero for managed λ)
  λi            ri  = 0;      // currently running λi (0 = main thread)
  bool          fin = false;  // we're done; ignore all requests
  u64           cs_ = 0;      // total context switches

  friend O &operator<<(O&, Λ&);
};


O &operator<<(O&, Λt const &);
O &operator<<(O&, Λ&);


}

#include "end.hh"


#endif
