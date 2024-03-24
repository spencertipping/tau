#include "websocket.hh"
#include "../begin.hh"

namespace σ
{


wsdecode &wsdecode::operator<<(Stc &x)
{
  b += x;
  while (!empty())
  {
    switch (s)
    {
    case state::header:  if (b.size() < 2)      return *this; parse_header(); break;
    case state::elen:    if (b.size() < elsize) return *this; parse_elen(); break;
    case state::mask:    if (b.size() < 4)      return *this; parse_mask(); break;
    case state::payload: if (b.size() < size)   return *this; emit(); break;
    }
  }
  return *this;
}


}

#include "../end.hh"
