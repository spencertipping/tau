#ifndef σserver_Ωio_h
#define σserver_Ωio_h

#define XXH_INLINE_ALL
#include <xxhash.h>

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
struct Ωf final
{
  Ωf(τ::Stc &path, int mode = 0644);
  ~Ωf();

  bool     read     (τ::u8  *b, τ::u64 s, τ::u64 o) const;
  bool     write    (τ::u8c *b, τ::u64 s, τ::u64 o);
  τ::u64   append   (τ::u8c *b, τ::u64 s);        // → o | -1
  τ::u8c  *mapped   (τ::u64  o, τ::u64 s) const;  // bounds-checked
  τ::u8c  *operator+(τ::u64  o) const;            // partially bounds-checked
  Tt T     get      (τ::u64  o) const { return *Rc<T const*>(*this + o); }
  τ::u8c  *map      ()          const;

  τ::Stc  &path     () const { return path_; }
  τ::u64   size     () const;  // fstat for file size
  τ::u64   mapsize  () const { return mapsize_; }
  bool     is_mapped() const { return map_ != nullptr; }
  void     unmap    () const;
  τ::fd_t  fd       () const;
  void     fsync    ();


protected:
  τ::Stc          path_;
  τ::fd_t         fd_;
  mutable τ::u8c *map_;
  mutable τ::u64  mapsize_;
  mutable bool    expanded_;
  τ::u64          pagesize_;

  τ::Sp<measurement>
    prof_read_,
    prof_write_,
    prof_append_,
    prof_fsync_;
};


// Memoized file open. We use this to avoid opening the same file multiple
// times, which also guarantees locking.
τ::Sp<Ωf> Ωfopen(τ::Stc &path);


typedef XXH128_hash_t                    ΩH;  // full hash
typedef decltype(XXH128_hash_t{}.high64) Ωh;  // partial hash
typedef τ::u64                           Ωp;  // packed offset+size

sletc Ωhm_size = 16;  // Ωh:u64b Ωp:u64b

inline ΩH Ωhash(τ::ηic &x)
{
  return XXH3_hashLong_128b_default(x.ldata(), x.lsize(), 0, nullptr, 0);
}


// Sorted stream of Ωh → pack pairs
// for (auto x = ss(); x; ++x) f(*x);
struct Ωss
{
  virtual ~Ωss() {}
  virtual operator          bool() const = 0;
  virtual τ::P<Ωh, Ωp> operator*() const = 0;
  virtual Ωss        &operator++()       = 0;

  // Maximum number of Ωh → pack pairs that can be generated from this stream
  virtual τ::u64        max_size() const = 0;
};


τ::Sp<Ωss> Ωss_frag (τ::Sp<Ωf>, τ::u64, τ::u64);
τ::Sp<Ωss> Ωss_merge(τ::Sp<Ωss>, τ::Sp<Ωss>);


}

#include "../end.hh"

#endif
