#include "Omega.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


void ΓΩ(Γφ&)
{
}


Ω::Ω(Stc &path, bool rw)
  : log_        (path + ".kv", rw),
    index_      (path + ".hm", rw),
    rw_         (rw),
    prof_add_   (measurement_for(ηm{} << "Ω" << path << "add")),
    prof_get_   (measurement_for(ηm{} << "Ω" << path << "get")),
    prof_commit_(measurement_for(ηm{} << "Ω" << path << "commit"))
{}


void Ω::add(ηic &k, ηic &v)
{
  ηm r;
  r.reserve(k.lsize() + 8 + v.lsize());
  r << k << v.all();
  TODO("Ω::add");
}


}

#include "../end.hh"
