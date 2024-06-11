#ifndef σserver_Ω_h
#define σserver_Ω_h

#include "Omega-types.hh"
#include "Omega-hm.hh"
#include "Omega-sr.hh"
#include "Omega-mr.hh"

#include "../prof.hh"
#include "../begin.hh"

namespace σ
{


void ΓΩ(Γφ&);


struct Ω final : Ωtypes
{
  Ω(Stc &path,
    f64  repack           = 0.5,
    u8   shard_bits       = 0,
    uN   max_staged_total = 64ull << 20);
  ~Ω();

  ηm   get(ηic&);
  Tt T get(ηic&, τ::Fc<T(fd_t, uN, uN)> &f);
  void set(ηic&, ηic&);
  void set(ηic&, ηm&&);
  void del(ηic&);


protected:


  void get_(ηic&, τ::Fc<void(fd_t, uN, uN)>&);
};


Tt T Ω::get(ηic &k, τ::Fc<T(fd_t, uN, uN)> &f)
{
  T r;
  get_(k, [&](fd_t fd, uN offset, uN len) { r = f(fd, offset, len); });
  return r;
}

Tn void Ω::get<void>(ηic &k, τ::Fc<void(fd_t, uN, uN)> &f)
{
  get_(k, f);
}


}

#include "../end.hh"

#endif
