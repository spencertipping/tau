#include "pi-int.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, πi const &i)
{
  s << "πi" << std::endl;
  s << "  is:";
  for (let &x : i.is()) s << " " << x;
  s << std::endl;

  s << "  os:";
  for (let &x : i.os()) s << " " << x;
  s << std::endl;

  s << "  rs:";
  for (let &x : i.rs()) s << " " << x;
  return s << std::endl;
}


}

#include "end.hh"
