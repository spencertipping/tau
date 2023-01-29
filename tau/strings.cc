#include "strings.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, cs7 const &c)
{
  s << "cs7[";
  for (u8 i = 0; i < 128; ++i) if (c[i]) s << i;
  return s << "]";
}


}

#include "end.hh"
