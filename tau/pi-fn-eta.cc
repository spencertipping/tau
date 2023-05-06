#include "pi-fn-eta.hh"
#include "begin.hh"

namespace τ
{


πmsf πηflatten()
{
  return [](πi &i, πv &&v) -> πvs
    { if (v.is_ntuple()) return πvs{*v.v()};
      πvs r;
      if (v.is_nmap())
      {
        r.reserve(v.m()->size() * 2);
        for (let &[k, x] : *v.m())
        {
          r.push_back(πv{k});
          r.push_back(v);
        }
        return r;
      }
      else if (v.t() == ηtype::η)
      {
        ηi x = v.η();
        r.push_back(x);
        while (x.has_next()) r.push_back(x = x.next());
        return r;
      }

      TODO("πηflatten other types (esp " << v.t() << ")");
      return r;
    };
}


}

#include "end.hh"
