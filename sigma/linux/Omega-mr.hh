#ifndef σserver_Ωmr_h
#define σserver_Ωmr_h

#include "Omega-io.hh"
#include "Omega-sr.hh"

#include "../begin.hh"

namespace σ
{


// Many sorted runs packed into a single file.
struct Ωmr final
{
  Tt using Sp = τ::Sp<T>;


protected:
  Sp<Ωf> f_;
};


}

#include "../end.hh"

#endif
