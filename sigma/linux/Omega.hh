#ifndef σserver_Ω_h
#define σserver_Ω_h

#include "../prof.hh"
#include "Omega-internals.hh"
#include "../begin.hh"

namespace σ
{


void ΓΩ(Γφ&);


struct Ω final
{
  using ηi    = τ::ηi;
  using ηic   = τ::ηic;
  using ηmc   = τ::ηmc;
  using ηm    = τ::ηm;
  using uN    = τ::uN;
  Tt using Sp = τ::Sp<T>;

  Ω(τ::Stc &path,
    τ::f64  repack           = 0.5,
    τ::u8   shard_bits       = 0,
    uN      max_staged_total = 64ull << 20);
  ~Ω();

  ηm   get(ηic&);
  Tt T get(ηic&, τ::Fc<T(τ::fd_t, uN, uN)> &f);
  void set(ηic&, ηic&);
  void set(ηic&, ηm&&);
  void del(ηic&);


protected:
  void get_(ηic&, τ::Fc<void(τ::fd_t, uN, uN)>&);
};


Tt T Ω::get(ηic &k, τ::Fc<T(τ::fd_t, uN, uN)> &f)
{
  T r;
  get_(k, [&](τ::fd_t fd, uN offset, uN len) { r = f(fd, offset, len); });
  return r;
}


}

#include "../end.hh"

#endif
