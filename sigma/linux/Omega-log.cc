#include "Omega.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


Ωl::Ωl(Stc &path, bool rw)
  : f_       (path, rw ? 0644 : 0444),
    rw_      (rw),
    prof_add_(measurement_for(ηm{} << "Ωl" << path << "add")),
    prof_get_(measurement_for(ηm{} << "Ωl" << path << "get"))
{
  if (rw_ && size() < 8)
    A(this->rw().write(Rc<u8c*>("Ωlog\0\0\1"), 8, 0),
      "Ωl " << path << ": failed to write header magic");
}


ηm Ωl::get(u64 k, u32 se) const
{
  ηm r;
  if (!se) se = size(k);
  r.x_.resize(se);
  let n = get(k, r.x_.data(), se);
  r.x_.resize(n);
  return r;
}


u64 Ωl::add(u8c *b, u32 s)
{
  let    t(prof_add_->start());
  Ul<Mu> l(add_mu_);
  auto  &f(rw());

  A(s > 0, "Ωl::add " << f.path() << ": value must have nonzero size");

  let    o(size());
  u32bc  sb(s);
  A(f.write(Rc<u8c*>(&sb), 4, o) && f.write(b, s, o + 4),
    "Ωl::add " << f.path() << ": failed to write value of size " << s);
  return o;
}


u32 Ωl::get(u64 k, u8 *b, u32 s) const
{
  let  t (prof_get_->start());
  let &f (ro());
  let  sb(size(k));
  A(s >= sb, "Ωl::get " << f.path() << ": undersized read: s = " << s << ", actual size = " << sb);
  A(f.read(b, sb, k + 4),
    "Ωl::get " << f.path() << ": failed to read value of maximum size " << s << ", real size " << sb);
  return sb;
}


}

#include "../end.hh"
