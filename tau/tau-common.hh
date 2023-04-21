#ifndef τ²common_h
#define τ²common_h


#include "types.hh"
#include "Lambda.hh"
#include "xi.hh"
#include "xiio.hh"

#include "begin.hh"

namespace τ
{


struct ψ_;


struct τΘ
{
  Θp h;
  λi l;

  // NOTE: PQ<ΦΘ> is a maxheap, so < == lower priority == later deadline
  bool operator<(τΘ const &x) const { return h > x.h; }
};


struct τb  // base τ
{
  τb(τb&)  = delete;
  τb(τb&&) = delete;
  τb() {}

  virtual ~τb() { fin_ = true; }

  Λ  &l()      { return l_; }
  ΔΘ  dt(Θp p) { return t0_ - p; }


  void Θ(Θp t)    { while (now() < t) { h_.push(τΘ{t, l_.i()}); l_.y(λs::Θ); } }
  Θp   hn() const { return h_.empty() ? forever() : h_.top().h; }
  ΔΘ   dt() const { return now() - t0_; }


  P<ξo, ξi> pipe(uN c = 8192) { let x = Sp<ξ>(new ξ(l_, c)); return mp(ξo(l_, x), ξi(x)); }

  Sp<ψ_> route (Stc &port)     const { return b_.contains(port) ? b_.at(port) : Sp<ψ_>{}; }
  bool   bind  (Stc &port, Sp<ψ_> q) { let c = b_.contains(port); b_[port] = q; return !c; }
  void   unbind(Stc &port)           { if (!fin_) b_.erase(port); }

  St gensym(Stc &pre) { return (Ss{} << "τgs_" << pre << gs_++).str(); }


  void pin  (Sp<ψ_> q) { qs_.insert(q); }
  void unpin(Sp<ψ_> q) { qs_.erase(q);  }

  uN pinned() const { return qs_.size(); }


protected:
  Λ             l_;            // Λ thread manager
  PQ<τΘ>        h_;            // timed threads
  M<St, Sp<ψ_>> b_;            // ψs with bound ports
  S<Sp<ψ_>>     qs_;           // boundary-pinned ψs
  u64           gs_  = 0;      // gensym ID
  bool          fin_ = false;  // if true, most operations are nops
  Θp const      t0_  = now();
};


O &operator<<(O &s, τΘ const &h);


}

#include "end.hh"


#endif
