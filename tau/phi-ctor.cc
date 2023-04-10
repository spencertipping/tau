#include "phi-ctor.hh"
#include "begin.hh"

namespace τ
{


φ<St> φCJK(uN min, uN max)
{
  // NOTE: char ranges from
  // https://stackoverflow.com/questions/1366068/whats-the-complete-range-for-chinese-characters-in-unicode
  return φucs([](u64 c)
    { return c >= 0x2f00  && c <= 0x2fdf
          || c >= 0x2e80  && c <= 0x2eff
          || c >= 0x3000  && c <= 0x303f
          || c >= 0x4e00  && c <= 0x9fcc
          || c >= 0x3400  && c <= 0x4db5
          || c >= 0x20000 && c <= 0x2a6df
          || c >= 0x2a700 && c <= 0x2b734
          || c >= 0x2b740 && c <= 0x2b81d; }, min, max);
}


}

#include "end.hh"
