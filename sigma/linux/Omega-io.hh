#ifndef σserver_Ωio_h
#define σserver_Ωio_h

#include <sys/mman.h>
#include <sys/stat.h>

#include "../prof.hh"
#include "../begin.hh"

namespace σ
{


struct Ωfd_ final
{
  Ωfd_ (τ::fd_t fd) : fd_(fd) {}
  ~Ωfd_()                     { if (ok()) close(fd_); }

  τ::fd_t fd()   const { return fd_; }
  bool    ok()   const { return fd_ != -1; }
  τ::u64  size() const
    { if (!ok()) return 0;
      struct stat s;
      A(fstat(fd_, &s) != -1, "Ωfd_(" << fd_ << ")::size() stat failed");
      return s.st_size; }

  void fdatasync() const { A(::fdatasync(fd_) != -1, "Ωfd::fdatasync() failed"); }
  void fsync()     const { A(::fsync(fd_)     != -1, "Ωfd::fsync() failed"); }

  void pread(void *buf, τ::u64 s, τ::u64 o = 0) const
  { A(::pread(fd_, buf, s, o) == τ::iS(s),
      "Ωfd::pread(fd=" << fd_ << ", s=" << s << ", o=" << o << ") failed"); }

  void pwrite(void const *buf, τ::u64 s, τ::u64 o = 0)
  { A(::pwrite(fd_, buf, s, o) == τ::iS(s),
      "Ωfd::pwrite(fd=" << fd_ << ", s=" << s << ", o=" << o << ") failed"); }

protected:
  τ::fd_t fd_;
};

typedef τ::Sp<Ωfd_> Ωfd;
typedef Ωfd const Ωfdc;


inline Ωfd Ωfd_manage(τ::fd_t fd) { return τ::Sp<Ωfd_>{new Ωfd_{fd}}; }
inline Ωfd Ωopen(τ::Stc &path, bool rw = false, int mode = 0644)
  { return Ωfd_manage(open(path.c_str(), rw ? O_RDWR | O_CREAT : O_RDONLY, mode)); }


struct Ωfl final  // file region lock
{
  Ωfl(Ωfd fd, bool rw = false) : fd_(fd), rw_(rw), locked_(false)
    { A(fd_->ok(), "Ωfl(" << fd_->fd() << ") failed: invalid fd"); }
  Ωfl(Ωfd fd, bool rw, τ::u64 o, τ::u64 s) : Ωfl(fd, rw) { lock(o, s); }
  ~Ωfl() { unlock(); }

  void lock(τ::u64 o, τ::u64 s) const
    { if (locked_) return;
      fl_.l_whence = SEEK_SET;
      fl_.l_start  = o;
      fl_.l_len    = s;
      fl_.l_type   = rw_ ? F_WRLCK : F_RDLCK;
      fl_.l_pid    = 0;  // NOTE: important (otherwise invalid argument)
      A(fcntl(fd_->fd(), F_OFD_SETLKW, &fl_) != -1,
        "fcntl lock(" << fd_->fd() << ", o=" << o << ", s=" << s << ", rw=" << rw_ << ") failed");
      locked_ = true; }

  void unlock() const
    { if (!locked_) return;
      fl_.l_type = F_UNLCK;
        A(fcntl(fd_->fd(), F_OFD_SETLKW, &fl_) != -1,
          "fcntl unlock(" << fd_->fd() << ") failed");
      locked_ = false; }

  bool locked() const { return locked_; }

protected:
  Ωfd                  fd_;
  bool                 rw_;
  mutable bool         locked_;
  mutable struct flock fl_;
};

typedef Ωfl const Ωflc;


// FIXME: linux automatically makes appended data available, so no need to remap
// for expanded files
//
struct Ωfm final  // file mmap; const means it represents the same file
{
  Ωfm (bool rw = false)         : fd_(),   map_(nullptr), mapsize_(0), rw_(rw) {}
  Ωfm (Ωfd fd, bool rw = false) : fd_(fd), map_(nullptr), mapsize_(0), rw_(rw) {}
  ~Ωfm() { unmap(); }


  bool  ok()        const { return fd_ != nullptr && (map_ != nullptr || fd_->ok()); }
  bool  is_mapped() const { return map_ != nullptr; }
  void *mapping()   const { map(); return map_; }

  void sync(τ::u64 o = 0, τ::u64 s = 0, bool fsync = false) const
    { if (!map()) return;
      if (o == 0 && s == 0) s = mapsize_;
      let ro = o & ~(getpagesize() - 1);  // align to page
      s += o - ro;
      A(msync((τ::ch*) map_ + ro, s, MS_SYNC) != -1,
        "msync(o=" << o << ", s=" << s << ") failed");
      if (fsync) fd_->fdatasync(); }

  bool ensure(τ::u64 o) const
    { if (o > mapsize_) update();
      return map() && o <= mapsize_; }

  void       *at       (τ::u64 o)       { return Cc<void*>(*this + o); }
  void const *operator+(τ::u64 o) const { return !ensure(o) ? nullptr : (τ::chc*) map_ + o; }

  void prefetch(τ::u64 o, τ::u64 s, int flags = 0) const
    { if (!map()) return;
      A(madvise((τ::ch*) map_ + o, s, MADV_WILLNEED | flags) != -1,
        "madvise() failed"); }


  bool map() const
    { if (is_mapped())        return true;
      if (!fd_ || !fd_->ok()) return false;
      if (let n = fd_->size();
          (map_ = mmap(nullptr, n, PROT_READ | (rw_ ? PROT_WRITE : 0),
                       MAP_SHARED, fd_->fd(), 0)) != MAP_FAILED)
        { mapsize_ = n; return true; }
      else
        { map_     = nullptr;
          mapsize_ = 0;
          return false; } }

  void unmap() const
  { if (is_mapped())
    { A(munmap(map_, mapsize_) != -1, "munmap() failed");
      map_     = nullptr;
      mapsize_ = 0; } }


  void update() const { if (mapsize_ != fd_->size())         unmap(); }
  void update(Ωfd fd) { if (!fd_ || fd->fd() != fd_->fd()) { unmap(); fd_ = fd; } }


protected:
  Ωfd             fd_;
  mutable void   *map_;
  mutable τ::u64  mapsize_;  // size of file when mapped
  bool            rw_;
};

typedef Ωfm const Ωfmc;


}

#include "../end.hh"

#endif
