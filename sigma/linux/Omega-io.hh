#ifndef σserver_Ωio_h
#define σserver_Ωio_h

#include "Omega-types.hh"
#include "Omega-hm.hh"

#include "../prof.hh"
#include "../begin.hh"

namespace σ
{


// A file that is both optionally mapped into memory and managed using
// traditional IO. We select the IO strategy based on the size of requests:
// anything smaller than a page is accessed through the map, and anything larger
// is retrieved using pread() to use the device's IO queue.
//
// You cannot modify memory in the mapping; i.e. it's const and we don't msync.
// If you want to modify the file, you must use write().
struct Ωf final : Ωtypes
{
  Ωf(Stc &path, int mode = 0644);
  ~Ωf();

  bool  read     (u8  *b, u64 s, u64 o) const;
  bool  write    (u8c *b, u64 s, u64 o);
  u64   append   (u8c *b, u64 s);        // → o | -1
  u8c  *mapped   (u64  o, u64 s) const;  // bounds-checked
  u8c  *operator+(u64  o) const;         // partially bounds-checked
  Tt T  get      (u64  o) const { return *Rc<T const*>(*this + o); }
  u8c  *map      ()       const;

  u64  size       () const;  // fstat for file size
  u64  mapped_size() const { return mapsize_; }
  bool is_mapped  () const { return map_ != nullptr; }
  void unmap      () const;
  void fsync      ();

  Ωm operator<<(ηic&);


protected:
  Stc          path_;
  fd_t         fd_;
  mutable u8c *map_;
  mutable u64  mapsize_;
  mutable bool expanded_;
  u64          pagesize_;
};


// Memoized file open. We use this to avoid opening the same file multiple
// times, which also guarantees locking.
τ::Sp<Ωf> Ωfopen(τ::Stc &path);


}

#include "../end.hh"

#endif
