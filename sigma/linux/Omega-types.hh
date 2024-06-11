#ifndef σserver_Ωtypes_h
#define σserver_Ωtypes_h

#include "../begin.hh"

namespace σ
{


struct Ωtypes
{
  Tt using Sp = τ::Sp<T>;
  Tt using V  = τ::V<T>;

  using ηi    = τ::ηi;
  using ηic   = τ::ηic;
  using ηmc   = τ::ηmc;
  using ηm    = τ::ηm;
  using uN    = τ::uN;
  using u8    = τ::u8;
  using u8c   = τ::u8c;
  using u64   = τ::u64;
  using u64b  = τ::u64b;
  using u64bc = τ::u64bc;
  using i64   = τ::i64;
  using f64   = τ::f64;
	using St    = τ::St;
	using Stc   = τ::Stc;
	using fd_t  = τ::fd_t;
};


}

#include "../end.hh"

#endif
