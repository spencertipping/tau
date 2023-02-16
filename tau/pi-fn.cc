#include "pi-fn.hh"
#include "begin.hh"

namespace τ
{


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
