#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "Omega.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


Ωl::Ωl(Stc &path, bool rw) : f_(path), rw_(rw)
{
  A((fd_ = Ωopen(f_, rw_, 0644))->ok(),
    "Ωl::Ωl: failed to open " << f_ << " with rw=" << rw_);
  map_.update(fd_);
  if (fd_->size() < 16) write_header(0);
}


ηm Ωl::get(key k) const
{
  ηm  m;
  let s = size(k);
  m.x_.resize(s);
  A(pread(fd_->fd(), m.x_.data(), s, k + 4) == s,
    "Ωl::get: failed to read " << s << " byte(s) of data");
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
  Ωfl  l(fd_, 8, 8, true);
  key  k = size();
  u32b s = x.lsize();
  let  e = k + 4 + s;
  fd_->pwrite(&s, 4, k);
  fd_->pwrite(x.ldata(), s, k + 4);
  write_header(e);
  map_.update();
  return k;
}


}

#include "../end.hh"
