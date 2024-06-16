#include <cstdlib>
#include <sys/stat.h>

#include "Omega.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


Ωf::Ωf(Stc &path, bool rw) : f_(path), rw_(rw)
{
  A((fd_ = Ωopen(f_, rw_))->ok(),
    "Ωf::Ωf(\"" << f_ << "\", rw=" << rw_ << "): open failed");

  if (rw && size() < 8) write_header_into(fd_);
  rev_ = rev();
}


u64 Ωf::size() const
{
  struct stat st;
  A(fstat(fd_->fd(), &st) == 0, "Ωf::size(): fstat failed");
  return st.st_size;
}


u32 Ωf::rev() const
{
  u32b rev;
  A(pread(fd_->fd(), &rev, 4, 4) == 4, "Ωf::rev(): failed to read revision");
  return rev;
}


void Ωf::write_header_into(Ωfd f) const
{
  A(pwrite(f->fd(), "Ωf\0\0\0\0\0", 8, 0) == 8,
    "Ωf::write_header_into(" << f->fd() << "): failed to write header");
}


Ωf &Ωf::operator=(ηic &x)
{
  A(rw_, "Ωf::operator=(ηic): read-only file");

  let fn = f_ + ".XXXXXXXX";
  St  ft = fn;
  let fd = Ωfd_manage(mkstemp(ft.data()));
  A(fd->ok(), "Ωf::operator=(ηic): mkstemp(\"" << fn << "\") failed");

  write_header_into(fd);
  A(pwrite(fd->fd(), x.ldata(), x.lsize(), 8) == x.lsize(),
    "Ωf::operator=(ηic): failed to write data");

  A(rename(ft.c_str(), f_.c_str()) == 0,
    "Ωf::operator=(ηic): rename(\"" << ft << "\", \"" << f_ << "\") failed");

  // Now that the new file is in place, signal all existing readers by bumping
  // the revision.
  u32b next_rev = rev() + 1;  // note: fd_ still refers to old file
  A(pwrite(fd_->fd(), &next_rev, 4, 4) == 4,
    "Ωf::operator=(ηic): failed to increase revision");
  fd_ = fd;
  return *this;
}


ηm Ωf::operator*() const
{
  // Verify that the revision is the same as it was. If not, then reopen the file.
  if (rev() != rev_)
  {
    A((fd_ = Ωfd_manage(open(f_.c_str(), rw_ ? O_RDWR : O_RDONLY)))->ok(),
      "Ωf::operator*(): failed to reopen " << f_ << ", rw=" << rw_);
    rev_ = rev();
  }

  ηm  r;
  let n = size();
  r.x_.resize(n - 8);
  A(pread(fd_->fd(), r.x_.data(), n - 8, 8) == n - 8,
    "Ωf::operator*(): failed to read data");
  return r;
}


}

#include "../end.hh"
