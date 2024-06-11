#ifndef σserver_Ωmr_h
#define σserver_Ωmr_h

#include "Omega-types.hh"
#include "Omega-io.hh"
#include "Omega-sr.hh"

#include "../begin.hh"

namespace σ
{


// Many sorted runs packed into a single file. On disk, the format looks like
// this:
//
//              +-------------------------+
//              |                         V
//              |                      |----|
// N:u64b | r1sr1l r2sr2l ... rNsrNl | sr1... sr2... ... srN...
//           |                         ^
//           +-------------------------+
//
// As usual, all numbers are u64b. Run metas are always densely-packed; we don't
// store empty metadata by setting rIl = 0 or anything.
struct Ωmr final : Ωtypes
{
  Ωmr(Sp<Ωf> f) : f_(f) {}

  u64   size()     const;
  Ωsrc &at  (uN)   const;
  Ωm    at  (ΩHc&) const;


protected:
  Sp<Ωf> f_;
  V<Ωsr> srs_;

  void decode_srs_();
};


}

#include "../end.hh"

#endif
