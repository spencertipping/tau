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

  τ::u64 size() const;
  τ::u32 rev () const;

  void write_header_into(Ωfd) const;

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
  τ::ηm  get       (key) const;
  key    next      (key k) const { return k + 4 + size(k); }
  τ::u64 offset    (key k) const { return k + 4; }
  τ::u64 size      ()      const { return *τ::uap<τ::u64bc>(map_ + 8); }
  τ::u32 size      (key k) const
    { let s = size();
      A(map_.ensure(k + 4) && k + 4 <= s, "Ωl: key " << k << " beyond EOF " << s);
      return *τ::uap<τ::u32bc>(map_ + k); }

  void write_header(τ::u64);

protected:
  τ::Stc f_;
  Ωfd    fd_;
  bool   rw_;
  Ωfm    map_;
};


}

#include "../end.hh"

#endif
