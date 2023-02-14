#include "pi-int.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, πv const &v)
{
  return v.is_η() ? s << v.as_η() : s << v.as_γ();
}


}

#include "end.hh"
