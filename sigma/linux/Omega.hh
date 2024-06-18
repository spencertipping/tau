#ifndef σserver_Ω_h
#define σserver_Ω_h

#include "Omega-io.hh"
#include "Omega-h.hh"

#include "../prof.hh"
#include "../begin.hh"

namespace σ
{


void ΓΩ(Γφ&);


struct Ωf final
{
  Ωf(τ::Stc&, bool rw = false);

  Ωf   &operator=(τ::ηic&);
  τ::ηm operator*() const;

  τ::u64 size             ()      const { return fd_->size(); }
  τ::u32 rev              ()      const { τ::u32b r; fd_->pread(&r, 4, 4); return r; }
  void   write_header_into(Ωfd f) const { f->pwrite("Ωf\0\0\0\0\0", 8, 0); }

protected:
  τ::Stc         f_;
  mutable Ωfd    fd_;
  mutable τ::u32 rev_;
  bool           rw_;
};


struct Ωl final
{
  Ωl(τ::Stc&, bool rw = false);

  typedef τ::u64 key;

  key    operator<<(τ::ηic&);
  τ::ηm  operator[](key)   const;
  τ::u64 offset    (key k) const { return k + 4; }
  τ::u64 size      ()      const { return *τ::uap<τ::u64bc>(map_ + 8); }
  τ::u32 size      (key k) const
    { let s = size();
      A(map_.ensure(k + 4) && k + 4 <= s, "Ωl: key " << k << " beyond EOF " << s);
      return *τ::uap<τ::u32bc>(map_ + k); }

  key next(key k) const
    { let s = size();
      let e = offset(k) + size(k);
      return offset(e) <= s ? e : 0; }

  struct it final
  {
    key       x;
    Ωl const &l;
    bool  operator==(it const &i) const { return x == i.x; }
    τ::ηm operator* ()            const { return l[x]; }
    it   &operator++()                  { x = l.next(x); return *this; }
  };
  it begin() const { return {16, *this}; }
  it end()   const { return {0,  *this}; }

  void sync() const { fd_->fsync(); }

  void write_header(τ::u64);

protected:
  τ::Stc f_;
  Ωfd    fd_;
  bool   rw_;
  Ωfm    map_;
};


struct Ωa final  // insert-only k/v store
{
  Ωa(τ::Stc&, bool rw = false);

  void  add(τ::ηic&, τ::ηic&);
  τ::ηm get(τ::ηic&) const;
  void  commit(bool fsync = false);

protected:
  Ωl                 l_;
  Ωh<τ::u64, τ::u64> h_;  // hash(k) → Ωl key

  τ::Sp<measurement>
    prof_ladd_,
    prof_hadd_,
    prof_hget_,
    prof_hresults_,
    prof_commit_,
    prof_fsync_;
};


}

#include "../end.hh"

#endif
