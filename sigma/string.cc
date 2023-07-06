#include <iomanip>

#include "string.hh"
#include "begin.hh"

namespace σ
{


St hex_encode(Stc &x) {
  Ss r;
  r << std::hex << std::setfill('0');
  for (ch c : x) r << std::setw(2) << int(u8(c));
  return r.str();
}

St hex_decode(Stc &x) {
  St r;
  for (uN i = 0; i < x.length(); i += 2)
    r += ch(std::stoi(x.substr(i, 2), nullptr, 16));
  return r;
}


void πstring(πφ &p)
{
  p .def_spre(">H", hex_encode)
    .def_spre("<H", hex_decode);
}


}

#include "end.hh"
