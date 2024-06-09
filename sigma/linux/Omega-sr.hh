#ifndef σserver_Ωsr_h
#define σserver_Ωsr_h

#include "Omega-hm.hh"
#include "Omega-io.hh"

#include "../begin.hh"

namespace σ
{


struct Ωsr;

typedef Ωsr const Ωsrc;


// A sorted run of HM pairs. On disk, the format looks like this:
// N:u64b [hm1][hm2]...[hmN], where hmI = H:u64b M:u64b.
//
// There can be at most 1T pairs in a run; the top 24 bits are used for flags.
struct Ωsr final
{
  sletc HAS_BLOOM = 1ull << 40;

  Tt using Sp = τ::Sp<T>;

  Ωsr()      = default;
  Ωsr(Ωsrc&) = default;
  Ωsr(Sp<Ωf> f, τ::u64 o) : f_(f), o_(o), n_(f_->get<τ::u64b>(o_)) {}

  Ωsr &operator=(Ωsrc&) = default;

  bool   has_bloom  ()       const { return n_ & HAS_BLOOM; }
  bool   may_contain(ΩHc&)   const;  // fast rejection, if available
  Ωm     at         (ΩHc&)   const;  // lookup, or return nonexistent
  Ωhm    at         (τ::u64) const;  // lookup by index
  τ::u64 size       ()       const { return n_ & 0x0000'00ff'ffff'ffffull; }

  τ::u64 lsearch(Ωh, τ::u64, τ::u64) const;
  τ::u64 bsearch(Ωh, τ::u64, τ::u64) const;
  τ::u64 isearch(Ωh, τ::u64, τ::u64) const;


protected:
  Sp<Ωf> f_;  // backing file for this data
  τ::u64 o_;  // offset of N within f_
  τ::u64 n_;  // flags + number of h→m pairs
};


}

#include "../end.hh"

#endif
