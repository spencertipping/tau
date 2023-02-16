#include "pi-int.hh"
#include "pi-fn.hh"
#include "begin.hh"

namespace τ
{


bool πi::run(uN o, F<bool(πi&, πinsn const&)> each)
{
  let rd = rdepth();
  rpush(o);
  while (rdepth() >= rd)
  {
    let j = rpeek()++;
    if (j >= f().fs.size())      return true;
    if (!each(*this, f().fs[j])) return false;
    if (!f().fs[j].f(*this))     return false;
  }
  return true;
}


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
