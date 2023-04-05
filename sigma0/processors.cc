#include "processors.hh"
#include "../begin.hh"

namespace τ
{


bool ξcp(ξi i, ξo o)
{
  for (let x : i) if (!(o << x)) return false;
  return true;
}


Γffn_ ξeach(F<void(η)> &&f, bool tap)
{
  return [tap, f=std::move(f)](Sp<ψ>, ξi i, ξo o)
    { for (let x : i)
      { f(x);
        if (tap && !(o << x)) break; }};
}


Γffn_ ξmap(F<η0o(η)> &&f)
{
  return [f=std::move(f)](Sp<ψ>, ξi i, ξo o)
    { for (let x : i) if (!(o << f(x))) break; };
}


Γffn_ ξτmap(F<η0o(η)> &&f)
{
  return [f=std::move(f)](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i) if (!(o << f(x)) || !(o << η1o(η1sig::τ))) break; };
}


}

#include "../end.hh"
