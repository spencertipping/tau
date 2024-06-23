#include "Omega.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


Ω1::Ω1(Stc &path, bool rw)
  : l_(path + ".k",  rw),
    h_(path + ".hl", rw),
    prof_add_   (measurement_for(ηm{} << "Ω1" << path << "add")),
    prof_commit_(measurement_for(ηm{} << "Ω1" << path << "commit")),
    prof_fsync_ (measurement_for(ηm{} << "Ω1" << path << "fsync"))
{}


P<Ωl::key, bool> Ω1::add(ηic &x)
{
  // Do we already have this entry? If so, return existing key; otherwise add
  // it.
  let     t    = prof_add_->start();
  Ωl::key k    = 0;
  let     h    = x.hash();
  let     miss = h_.get(h, [&](u64b l)
    { let y = l_[l];
      if (y.y() == x) { k = l; return false; }
      else            return true; });

  if (!miss) return {k, false};

  k = l_ << x;
  h_.add(h, k);
  return {k, true};
}


void Ω1::commit(bool fsync)
{
  let t = (fsync ? prof_fsync_ : prof_commit_)->start();
  if (fsync) l_.sync();
  h_.commit(fsync);
}


}

#include "../end.hh"
