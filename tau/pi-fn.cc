#include "pi-fn.hh"
#include "begin.hh"

namespace τ
{


bool πfn::run(πi &i)
{
  let rd = i.rdepth();
  i.rpush(0);
  while (i.rdepth() >= rd)
  {
    let j = i.rpeek()++;
    if (j >= fs.size()) return true;
    if (!fs[j].f(i))    return false;
  }
  return true;
}


O &operator<<(O &s, πinsn const &i)
{
  return s << i.name;
}


O &operator<<(O &s, πfn const &f)
{
  for (uN i = 0; i < f.fs.size(); ++i)
    s << i << "\t" << f.fs[i] << std::endl;
  return s;
}


}

#include "end.hh"
