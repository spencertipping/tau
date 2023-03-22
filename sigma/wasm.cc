#include "wasm.hh"
#include "begin.hh"

namespace τ
{


#if τwasm
void φwasm(φd_<πfn> &f)
{
  std::cerr << "TODO: wasm Γ/φ library" << std::endl;
}
#else
void φwasm(φd_<πfn>&) {}
#endif


}

#include "end.hh"
