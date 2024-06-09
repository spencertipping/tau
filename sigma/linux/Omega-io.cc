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


Ωf::Ωf(Stc &path, int mode, u64 append_buffer_size)
  : fd_(-1),
    map_(nullptr),
    mapsize_(0),
    expanded_(false),
    pagesize_(getpagesize()),
    append_buffer_size_(append_buffer_size)
{
  A((fd_ = open(path.c_str(), O_CREAT | O_RDWR, mode)) != -1,
    "Ωf open() failed: " << strerror(errno));
  A((pagesize_ & pagesize_ - 1) == 0,
    "Ωf getpagesize() returned non-power-of-two (wtf is your system): " << pagesize_);
  append_buffer_.reserve(append_buffer_size_);
}


Ωf::~Ωf()
{
  commit();
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
  // FIXME: consult append buffer if necessary; otherwise reads are not coherent

  // If we already have a map and we're reading within one page, just copy out
  // from there.
  if (map_ && s <= pagesize_)
  {
    A(o + s <= mapsize_, "Ωf read() out of bounds: " << o + s << " > " << mapsize_);
    memcpy(b, map_ + o, s);
    return true;
  }

  for (u64 n = 0; n < s;)
    if      (let r = pread(fd_, b + n, s - n, o + n); r != -1) n += r;
    else if (errno != EINTR) return false;
  return true;
}


bool Ωf::write(u8c *b, u64 s, u64 o)
{
  expanded_ |= o + s > size();

  // No optimization for writes, since they tend to be less frequent and would
  // require msync().
  for (u64 n = 0; n < s;)
    if      (let r = pwrite(fd_, b + n, s - n, o + n); r != -1) n += r;
    else if (errno != EINTR) return false;
  return true;
}


u64 Ωf::append(u8c *b, u64 s)
{
  expanded_ |= s > 0;
  if (s >= append_buffer_size_)
  {
    commit();
    let r = size();
    A(write(b, s, r), "Ωf append() failed: " << strerror(errno));
    return r;
  }
  else if (append_buffer_.size() + s > append_buffer_size_)
  {
    commit();
    append_buffer_.assign(b, b + s);
    return size();
  }
  else
  {
    append_buffer_.insert(append_buffer_.end(), b, b + s);
    return size();
  }
}


u8c *Ωf::mapped(u64 o, u64 s)
{
  map();
  A(o + s <= mapsize_, "Ωf::mapped() out of bounds: " << o + s << " > " << mapsize_);
  return map_ + o;
}


u8c *Ωf::operator+(u64 o)
{
  map();
  A(o <= mapsize_, "Ωf::operator+() out of bounds: " << o << " > " << mapsize_);
  return map_ + o;
}


u8c *Ωf::map()
{
  if (map_ && !expanded_) return map_;
  if (map_)
  {
    A(munmap((void*) map_, mapsize_) != -1,
      "Ωf munmap() failed: " << strerror(errno));
    map_ = nullptr;
  }

  let s = size();
  if (s == 0) return nullptr;

  mapsize_ = (s + pagesize_ - 1) & -pagesize_;
  A((map_ = (u8c*) mmap(nullptr, mapsize_, PROT_READ, MAP_SHARED, fd_, 0)) != MAP_FAILED,
    "Ωf mmap() failed: " << strerror(errno));
  expanded_ = false;
  return map_;
}


void Ωf::commit(bool fsync)
{
  if (append_buffer_.empty()) return;
  A(write(append_buffer_.data(), append_buffer_.size(), size()),
    "Ωf commit() failed: " << strerror(errno));
  append_buffer_.clear();

  if (fsync) A(::fsync(fd_) != -1, "Ωf fsync() failed: " << strerror(errno));
}


Ωm Ωf::operator<<(ηic &x)
{
  // FIXME: large objects need to have two bytes of size-residue prefix
  let s = x.lsize();
  let o = append(x.ldata(), s);
  A(o != -1, "Ωf << failed");
  return Ωm(o, s);
}


}

#include "../end.hh"
