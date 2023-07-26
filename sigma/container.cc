#include "container.hh"
#include "begin.hh"

namespace σ
{


slet container_ = Ψauto([](pre::ctype t, pre::cback b, ξi i, ξo o)
  {
    let c = pre::at(t, b);
    for (let x : i)
      if (x.is_τ()) c->τ(x.next(), o);
      else
        switch (x.next().sig())
        {
        case ηsig::α: c->α(x.one(), x.next().next(), o); break;
        case ηsig::ω: c->ω(x.one(), x.next().next(), o); break;
        case ηsig::κ: c->κ(x.one(), x.next().next(), o); break;
        case ηsig::ι: c->ι(x.one(), x.next().next(), o); break;
        case ηsig::ρ: c->ρ(x.one(), x.next().next(), o); break;
        default:
          A(0, "@ " << x << ": invalid");
        }
  });


Γ container(pre::ctype t, pre::cback b, Ψd d)
{
  return ΓΨ(container_(t, b), d, "@");
}


void Γcontainer(Γφ &g)
{
  g .def_p2("@", Ψauto(container_));
}


}

#include "end.hh"
