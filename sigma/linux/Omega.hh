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
  Ωl(τ::Stc&, bool rw = false);

  τ::u64 add (τ::ηic &x) { return add(x.ldata(), x.lsize()); }
  τ::ηm  get (τ::u64 k, τ::u32 se = 0) const;
  τ::ηi  peek(τ::u64 k) const { return {f_ + offset(k), size(k)}; }

  τ::u64 add   (τ::u8c*, τ::u32);  // returns k
  τ::u32 get   (τ::u64 k, τ::u8*, τ::u32) const;
  τ::u64 offset(τ::u64 k)                 const { return k + 4; }
  τ::u32 size  (τ::u64 k)                 const { return ro().get<τ::u32b>(k); }
  τ::u64 next  (τ::u64 k)                 const { return offset(k) + size(k); };

  τ::u64  size() const { return f_.size(); }
  τ::fd_t fd()   const { return f_.fd(); }

  Ωf       &rw()       { A(rw_, "Ωl::rw() on read-only file " << f_.path()); return f_; }
  Ωf const &ro() const { return f_; }

protected:
  Ωf    f_;
  bool  rw_;  // false = read-only
  τ::Mu add_mu_;

  τ::Sp<measurement>
    prof_add_,
    prof_get_;
};


struct Ωi final
{
  typedef ΩH                   key;
  typedef τ::P<τ::u64, τ::u32> val;

  static Ωp  pack  (τ::u64 offset, τ::u32 size);
  static val unpack(Ωp packed);

  Ωi(τ::Stc&, bool rw = false, τ::u64 mss = 1048576);

  void         add(key, τ::u64 offset, τ::u32 size);
  τ::V<val>    get(key) const;
  τ::u64       size()   const { return ro().size(); }
  τ::u64       k()      const { return fs_.size(); }
  τ::V<τ::u64> ks()     const;
  τ::fd_t      fd()     const { return f_.fd(); }

  τ::u64 real_hkey  () const { return ro().get<τ::u64 volatile>(8); }
  τ::u64 real_hrev  () const { return ro().get<τ::u64 volatile>(16); }
  τ::u64 stored_hkey() const { return hkey_; }
  τ::u64 stored_hrev() const { return hrev_; }

  bool reader_valid() const { return real_hrev() == stored_hrev(); }
  bool writer_valid() const { return real_hkey() == stored_hkey(); }
  void repack(τ::u64 max_bytes);
  void commit();

  Ωf       &rw()       { A(rw_, "Ωi::rw() on read-only file " << f_.path()); return f_; }
  Ωf const &ro() const { return f_; }

protected:
  Ωf                         f_;
  mutable τ::Smu             fs_mu_;
  τ::V<τ::P<τ::u64, τ::u64>> fs_;     // fragments
  τ::u64                     hkey_;   // header key
  τ::u64                     hrev_;   // header revision

  // TODO: remove stage; it doesn't belong in this abstraction layer
  // (it should be in Ω instead)
  mutable τ::Smu             stage_mu_;
  τ::M<Ωh, Ωp>               stage_;  // staged but uncommitted data
  τ::u64                     mss_;    // maximum stage size
  bool                       rw_;     // false = read-only

  τ::Sp<measurement>
    prof_add_,
    prof_get_,
    prof_repack_,
    prof_commit_;

  void       read_fragment_table_();
  void       write_hkey_();
  void       commit_();               // assumes stage_mu_ is held
  τ::Sp<Ωss> fragment_(τ::u64) const;
  void       fragment_(τ::Sp<Ωss>);   // allocate new fragment, update header
};


struct Ω final
{
  Ω(τ::Stc &path, bool rw = false);
  ~Ω();

  void  add(τ::ηic &k, τ::ηic &v);
  τ::ηm get(τ::ηic &k) const;
  void  commit(bool fsync = false);
  Ωl   &log();
  Ωi   &index();
  bool  is_valid() const;

protected:
  Ωl   log_;
  Ωi   index_;
  bool rw_;

  τ::Sp<measurement>
    prof_add_,
    prof_get_,
    prof_commit_;
};


}

#include "../end.hh"

#endif
