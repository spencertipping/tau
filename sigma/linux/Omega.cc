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
  index_.add(Ωhash(k), log_.add(r), r.lsize());
}


ηm Ω::get(ηic &k) const
{
  // We'll have multiple candidates if the hash collides. In that case, search
  // each until we find the matching key.
  for (let &[o, s] : index_.get(Ωhash(k)))
  {
    // Peek at the key without copying the whole k/v pair. This will be
    // optimally fast if the key fits into a single page, which is almost
    // always.

  }
}


}

#include "../end.hh"
