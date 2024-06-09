#ifndef σserver_Ωsr_h
#define σserver_Ωsr_h

#include "Omega-hm.hh"
#include "Omega-io.hh"

#include "../begin.hh"

namespace σ
{


// A sorted run of HM pairs.
struct Ωsr final
{
  Tt using Sp = τ::Sp<T>;

  Ωsr() = default;
  Ωsr(Sp<Ωf> f, τ::u64 o) : f_(f), o_(o) {}

  Ωsr &operator=(const Ωsr &x) { f_ = x.f_; o_ = x.o_; return *this; }

  bool may_contain(ΩHc&) const;


protected:
  Sp<Ωf> f_;
  τ::u64 o_;  // first byte
};


}

#include "../end.hh"

#endif
