#include "../pre-container.hh"
#include "../begin.hh"

namespace σ::pre
{


Sp<at_> lmdb_set(cb_lmdb const&)
{
  A(0, "lmdb_set unsupported on wasm");
  return {};
}

Sp<at_> lmdb_map(cb_lmdb const&)
{
  A(0, "lmdb_map unsupported on wasm");
  return {};
}


Sp<at_ls_> lmdb_ls(cb_lmdb const&)
{
  A(0, "lmdb_ls unsupported on wasm");
  return {};
}


}

#include "../end.hh"
