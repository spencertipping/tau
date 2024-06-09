#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Omega-io.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


Ωf::Ωf(Stc &path, int mode)
{
  A((fd_ = open(path.c_str(), O_CREAT | O_RDWR, mode)) != -1,
    "Ωf open() failed: " << strerror(errno));
  map_      = nullptr;
  mapsize_  = 0;
  pagesize_ = getpagesize();
}


Ωf::~Ωf()
{
  if (fd_ != -1) close(fd_);
  fd_ = -1;
}


u64 Ωf::size() const
{
  struct stat s;
  A(fstat(fd_, &s) != -1, "Ωf fstat() failed: " << strerror(errno));
  return s.st_size;
}


bool Ωf::read(u8 *b, u64 s, u64 o)
{
  // If we already have a map and we're reading less than a page, just copy out
  // from there.

  u64 n = 0;
  while (n < s)
  {

  }
}


}

#include "../end.hh"
