#ifndef τΞΓ_h
#define τΞΓ_h


#include "Xi.hh"
#include "Gamma.hh"

#include "begin.hh"

namespace τ
{


inline Γ ΞΓdrop() { return new Γf_{"Ξ_", [](Ξc &x) { return x.drop(); }}; }
inline Γ ΞΓpush() { return new Γf_{"Ξ+", [](Ξc &x) { return x.push({}); }}; }

inline Γ ΞΓswap()
{
  return new Γf_{"%", [](Ξc &x)
  { let [a, y] = x.pop();
    let [b, z] = y.pop();
    return z.push(a).push(b); }};
}


inline Γ ΞΓvs(St n)
{
  return new Γf_{":" + n, [n](Ξc &x)
    { let [a, y] = x.pop(); return y.vs(n, a); }};
}

inline Γ ΞΓvg(St n)
{
  return new Γf_{"@" + n, [n](Ξc &x)
    { return x.push(x.vg(n)); }};
}


inline Γ ΞΓdebug(St tag)
{
  return new Γf_{"debug[" + tag + "]", [tag](Ξc &x)
    { std::cout << "debug " << tag << ": " << x << std::endl;
      return x; }};
}


}

#include "end.hh"


#endif
