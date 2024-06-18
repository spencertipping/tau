#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "Omega.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


Ωl::Ωl(Stc &path, bool rw) : f_(path), rw_(rw), map_(rw_)
{
  A((fd_ = Ωopen(f_, rw_, 0644))->ok(),
    "Ωl::Ωl: failed to open " << f_ << " with rw=" << rw_);
  map_.update(fd_);
  if (fd_->size() < 16) write_header(16);
}


ηm Ωl::operator[](key k) const
{
  ηm  m;
  let s = size(k);
  m.x_.resize(s);
  fd_->pread(m.x_.data(), s, offset(k));
  return m;
}


void Ωl::write_header(u64 s)
{
  u64b sb = s;
  fd_->pwrite("Ωl\0\0\0\0\0", 8, 0);
  fd_->pwrite(&sb, 8, 8);
}


Ωl::key Ωl::operator<<(ηic &x)
{
  Ωfl  l(fd_, true, 8, 8);
  key  k = size();
  u32b s = x.lsize();
  let  e = offset(k) + s;
  fd_->pwrite(&s, 4, k);
  fd_->pwrite(x.ldata(), s, offset(k));
  write_header(e);
  map_.update();
  return k;
}


}

#include "../end.hh"
