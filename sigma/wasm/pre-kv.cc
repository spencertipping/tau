#include "../pre-kv.hh"
#include "../../tau/begin.hh"

namespace σ::pre
{


Sp<kv_> kv_lmdb(Stc&, Stc&)
{
  A(0, "LMDB unsupported on wasm");
  return {};
}


Sp<kv_> kv_sqlite(Stc&, Stc&)
{
  A(0, "SQLite unsupported on wasm");
  return {};
}


}

#include "../../tau/end.hh"
