#include <iostream>

#include "io.hh"
#include "../begin.hh"

namespace τ
{


struct γostream_ : public virtual γ
{
  γostream_(O &s_) : s(s_) {}

  St name() const { return "γ»"; }

  Ξ &operator()(Ξ &x)
    { let q = Sp<ψ>(new ψ(x.t()));
      let [i, o] = x.xf(q);
      q->name(name())
        .def([&s=s, i=i, o=o]() mutable
          { for (let x : i)
            { s << η1i{x} << std::endl;
              o << x; }});
      return x; }

protected:
  O &s;
};


// TODO: rework as γffn
Sp<γ> γostream(O &s) { return Sp<γ>{new γostream_(s)}; }


}

#include "../end.hh"
