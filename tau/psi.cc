#include "debug.hh"
#include "psi.hh"
#include "begin.hh"

namespace τ
{

#if τtrack_ψ
  static S<ψ_*> ψs_;

  void ψc_(ψ_ *q) { ψs_.insert(q); }
  void ψx_(ψ_ *q) { ψs_.erase(q); }
  uN   ψn()       { return ψs_.size(); }

  S<ψ_*> &ψs()    { return ψs_; }
#else
  static iN ψs_ = 0;

  void ψc_(ψ_ *q) { ++ψs_; }
  void ψx_(ψ_ *q) { A(--ψs_ >= 0, "ψs underflow"); }
  uN   ψn()       { return ψs_; }
#endif


void ψ_::soft_destroy()
{
  for (let &f : df_) f(*this);
  hard_destroy();
}


void ψ_::hard_destroy()
{
  // NOTE: two destinations for this λ depending on where it came from.
  // If we created it, then it should return once it's done with whatever
  // destruction it's doing here. We should let it live because it may be
  // chain-destructing other ψs.
  //
  // If someone else created it, then we should let it live because it's
  // deallocating ψs referenced by another ψ.

  Λ &l = t_.l();
  for (let  x      : ls_) if (l.e(x) && x != l.i()) l.x(x);
  for (let &[p, _] : cs_)                           t_.unbind(p);
  ls_.clear();
  cs_.clear();
  df_.clear();
}


ψ_ &ψ_::f(λf &&f)
{
  Λ &l = t_.l();
  for (auto i = ls_.begin(); i != ls_.end();)
    if (!l.e(*i)) i = ls_.erase(i);
    else          ++i;
  ls_.insert(l.c(mo(f)));
  return *this;
}


O &operator<<(O &s, ψ const &q)
{
  return s << q.name();
}


O &operator<<(O &s, ψ_ const &q)
{
  return s << q.n_;
}


}

#include "end.hh"
