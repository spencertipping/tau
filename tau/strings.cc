#include "../tau.hh"
#include "begin.hh"

namespace τ
{


V<uN> cs7::find(Stvc &s) const
{
  V<uN> r;
  for (uN i = 0; i < s.size(); ++i)
    if ((*this)[s[i]]) r.push_back(i);
  return r;
}


V<Stv> cs7::split(Stvc &s) const
{
  V<uN> is = find(s);
  V<Stv> r;
  if (is.size())
  {
    r.push_back(s.substr(0, is.front()));
    for (uN i = 0; i + 1 < is.size(); ++i)
      r.push_back(s.substr(is[i] + 1, is[i + 1] - is[i] - 1));
    r.push_back(s.substr(is.back() + 1));
  }
  else
    r.push_back(s);
  return r;
}


V<Stv> ssplit(St p, Stvc &s)
{
  V<Stv> r;
  uN l = 0;
  for (uN i = 0; (i = s.find(p, l)) != St::npos; l = i + p.size())
    r.push_back(s.substr(l, i - l));
  r.push_back(s.substr(l));
  return r;
}


O &operator<<(O &s, cs7 const &c)
{
  s << "cs7[";
  for (u8 i = 0; i < 128; ++i) if (c[i]) s << i;
  return s << "]";
}


}

#include "end.hh"
