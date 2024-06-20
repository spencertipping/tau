#include "Omega.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


Ω1::Ω1(Stc &path, bool rw)
  : l_(path + ".k",  rw),
    h_(path + ".hm", rw),
    prof_ladd_  (measurement_for(ηm{} << "Ω1" << path << "Ωl::add")),
    prof_hadd_  (measurement_for(ηm{} << "Ω1" << path << "Ωh::add")),
    prof_hget_  (measurement_for(ηm{} << "Ω1" << path << "Ωh::get")),
    prof_commit_(measurement_for(ηm{} << "Ω1" << path << "commit")),
    prof_fsync_ (measurement_for(ηm{} << "Ω1" << path << "fsync"))
{}


Ωl::key Ω1::add(ηic &x)
{
  TODO("Ω1::add");
}


void Ω1::commit(bool fsync)
{
  let t = (fsync ? prof_fsync_ : prof_commit_)->start();
  if (fsync) l_.sync();
  h_.commit(fsync);
}


}

#include "../end.hh"
