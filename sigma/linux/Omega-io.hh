#ifndef σserver_Ωio_h
#define σserver_Ωio_h

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
// You cannot modify memory in the mapping. If you do (disregarding const), your
// changes will be lost.
struct Ωf final
{
  Ωf() = delete;
  Ωf(τ::Stc &path, int mode = 0644);
  ~Ωf();

  bool    read     (τ::u8  *b, τ::u64 s, τ::u64 o);
  bool    write    (τ::u8c *b, τ::u64 s, τ::u64 o);
  τ::u64  append   (τ::u8c *b, τ::u64 s);  // → o | -1
  τ::u8c *mapped   (τ::u64  o, τ::u64 s);  // bounds-checked
  τ::u8c *operator+(τ::u64  o);            // partially bounds-checked
  Tt T    get      (τ::u64  o) { return *Rc<T const*>(*this + o); }

  τ::u64  size()        const;              // fstat for file size
  τ::u64  mapped_size() const { return mapsize_; }
  bool    is_mapped()   const { return map_ != nullptr; }

  void    commit(bool fsync = false);       // write appended data

  Ωm operator<<(τ::ηic&);


protected:
  τ::fd_t fd_;
  τ::u8c *map_;
  τ::u64  mapsize_;
  τ::u64  pagesize_;
};


// Memoized file open. We use this to avoid opening the same file multiple
// times, which also guarantees locking.
τ::Sp<Ωf> Ωfopen(τ::Stc &path);


}

#include "../end.hh"

#endif
