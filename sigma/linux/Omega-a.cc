#include "Omega.hh"
#include "../prof.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


Ωa::Ωa(Stc &path, bool rw)
  : l_(path + ".kv", rw),
    h_(path + ".hm", rw),
    prof_ladd_    (measurement_for(ηm{} << "Ωa" << path << "Ωl::add")),
    prof_hadd_    (measurement_for(ηm{} << "Ωa" << path << "Ωh::add")),
    prof_hget_    (measurement_for(ηm{} << "Ωa" << path << "Ωh::get")),
    prof_hresults_(measurement_for(ηm{} << "Ωa" << path << "results")),
    prof_commit_  (measurement_for(ηm{} << "Ωa" << path << "commit")),
    prof_fsync_   (measurement_for(ηm{} << "Ωa" << path << "fsync"))
{}


void Ωa::add(ηic &k, ηic &v)
{
  auto lat = prof_ladd_->start();
  let  lk  = l_ << (ηm{} << k << v.all());
  lat.stop();

  let hat = prof_hadd_->start();
  h_.add(k.hash(), lk);
}


ηm Ωa::get(ηic &k) const
{
  auto hgt = prof_hget_->start();
  u64 os[256];
  let n = h_.get(k.hash(), os, sizeof(os) / sizeof(os[0]));
  hgt.stop();

  let hrt = prof_hresults_->start();
  for (u32 i = 0; i < n; ++i)
    if (let x = l_[os[i]]; x.y().η() == k) return x;
  return {};
}


void Ωa::commit(bool fsync)
{
  let t = (fsync ? prof_fsync_ : prof_commit_)->start();
  if (fsync) l_.sync();
  h_.commit(fsync);
}


}

#include "../end.hh"
