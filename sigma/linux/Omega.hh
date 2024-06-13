#ifndef σserver_Ω_h
#define σserver_Ω_h

#include <xxhash.h>

#include "Omega-io.hh"

#include "../prof.hh"
#include "../begin.hh"

namespace σ
{


void ΓΩ(Γφ&);


struct Ωl final
{
  τ::u64 add(τ::ηic&);
  void   del(τ::u64);
  τ::ηm  get(τ::u64) const;

  τ::u64 add   (τ::u8c*, τ::u32);  // returns k
  τ::u32 get   (τ::u64 k, τ::u8*, τ::u32) const;
  τ::u64 offset(τ::u64 k)                 const { return k + 4; }
  τ::u32 size  (τ::u64 k)                 const;
  τ::u64 next  (τ::u64 k)                 const;

  τ::fd_t fd() const;
  void    repack();

  Ωf       &rw()       { A(rw_, "Ωl::rw() on read-only file " << f_.path()); return f_; }
  Ωf const &ro() const { return f_; }

protected:
  Ωf   f_;
  bool rw_;  // false = read-only
};


struct Ωi final
{
  typedef XXH128_hash_t ΩH;
  typedef ΩH                   key;
  typedef τ::P<τ::u64, τ::u32> val;

  static τ::u64 pack  (τ::u64 offset, τ::u32 size);
  static val    unpack(τ::u64 packed);

  void         add(key, τ::u64 offset, τ::u32 size);
  void         del(key);
  τ::V<val>    get(key)      const;
  ΩH           hash(τ::ηic&) const;
  τ::u64       size()        const;
  τ::u64       k()           const;
  τ::V<τ::u64> ks()          const;
  τ::fd_t      fd()          const;

  τ::u64 real_hkey  () const;
  τ::u64 stored_hkey() const;
  τ::u64 real_hrev  () const;
  τ::u64 stored_hrev() const;

  bool reader_valid() const { return real_hrev() == stored_hrev(); }
  bool writer_valid() const { return real_hkey() == stored_hkey(); }
  void repack(τ::u64 max_bytes);
  void commit();

  Ωf       &rw()       { A(rw_, "Ωi::rw() on read-only file " << f_.path()); return f_; }
  Ωf const &ro() const { return f_; }

protected:
  Ωf     f_;
  τ::u64 hkey_;  // header key
  τ::u64 hrev_;  // header revision
  bool   rw_;    // false = read-only
};


struct Ω final
{
  void  add(τ::ηic &k, τ::ηic &v);
  void  del(τ::ηic &k);
  τ::ηm get(τ::ηic &k) const;
  void  commit(bool fsync);

  Ωl &log();
  Ωi &index();

  bool is_valid() const;

protected:
  Ωl   log_;
  Ωi   index_;
  bool rw_;
};


}

#include "../end.hh"

#endif
