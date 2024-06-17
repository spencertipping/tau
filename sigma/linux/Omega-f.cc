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


Ωf &Ωf::operator=(ηic &x)
{
  A(rw_, "Ωf::operator=(ηic): read-only file");

  let fn = f_ + ".XXXXXXXX";
  St  ft = fn;
  let fd = Ωfd_manage(mkstemp(ft.data()));
  A(fd->ok(), "Ωf::operator=(ηic): mkstemp(\"" << fn << "\") failed");

  write_header_into(fd);
  fd->pwrite(x.ldata(), x.lsize(), 8);

  A(rename(ft.c_str(), f_.c_str()) == 0,
    "Ωf::operator=(ηic): rename(\"" << ft << "\", \"" << f_ << "\") failed");

  // Now that the new file is in place, signal all existing readers by bumping
  // the revision.
  u32b next_rev = rev() + 1;  // note: fd_ still refers to old file
  fd_->pwrite(&next_rev, 4, 4);
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
  fd_->pread(r.x_.data(), n - 8, 8);
  return r;
}


}

#include "../end.hh"
