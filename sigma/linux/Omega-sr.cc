#include "Omega-sr.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


bool Ωsr::may_contain(ΩHc &h) const
{
  // TODO: bloom stuff
  return true;
}


Ωhm Ωsr::at(u64 i) const
{
  A(i + 16 <= size(), "Ωsr::at: " << i << " + 16 > " << size());
  return Ωhm(*f_ + (i*16 + 8));
}


Ωm Ωsr::at(ΩHc &h) const
{
  let i = isearch(h.high64, 0, size());
  return i == -1ull ? Ωm() : at(i).meta();
}


u64 Ωsr::lsearch(Ωh h, u64 l, u64 u) const
{
  for (u64 i = l; i < u; i++)
  {
    let hi = at(i).hash();
    if      (hi == h) return  i;
    else if (hi >  h) return -1;
  }
  return -1;
}


u64 Ωsr::bsearch(Ωh h, u64 l, u64 u) const
{
  while (l + 8 < u)
  {
    u64 m  = l + u >> 1;
    let hm = at(m);
    if      (hm.hash() < h) l = m + 1;
    else if (hm.hash() > h) u = m;
    else                    return m;
  }
  return lsearch(h, l, u);
}


u64 Ωsr::isearch(Ωh h, u64 l, u64 u) const
{
  --u;
  for (Ωh hl = at(l).hash(),
          hu = at(u).hash();
       hu > hl && l + 8 < u;)
  {
    let m  = (h - hl) / (hu - hl) * (u - l) + l;
    let hi = at(m).hash();
    if      (hi < h) { l = m + 1; hl = hi; }
    else if (hi > h) { u = m;     hu = hi; }
    else             return m;
  }

  return lsearch(h, l, u + 1);
}


}

#include "../end.hh"
