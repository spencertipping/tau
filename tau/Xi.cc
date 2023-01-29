#include "Xi.hh"
#include "begin.hh"

namespace τ
{


Ξ Ξ::sel(Re const &r)
{
  Ξ x{l()};
  for (let &[k, v] : xs_) if (std::regex_match(k, r)) x.xs_[k] = v;
  return x;
}

Ξ Ξ::sel(Re const &r, Ξ const &x)
{
  Ξ y{l()};
  for (let &[k, v] : xs_) if (!std::regex_match(k, r)) y.xs_[k] = v;
  for (let &[k, v] : x)   y.xs_[k] = v;
  return y;
}


O &operator<<(O &s, Ξ const &x)
{
  s << "Ξ[";
  bool first = true;
  for (let &[k, io] : x)
  { if (first) first = false;
    else       s << " ";
    s << k << "|" << (io->can_i() ? "i" : "")
      << (io->can_o() ? "o" : ""); }
  return s << "]";
}


}

#include "end.hh"
