#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_Γ(πφ &p)
{
  p .def_ppost("G>", [](πi &i, πP<Γa<Γ>> g, ηic &x)
    { A(i.t(), "π G> with no τe");
      ξi r;
      {
        let [fo, fi] = i.t()->pipe();
        let s = Ξ{*i.t()}.push(ξdc{fi, {}});
        r = (*g).x(s).f();
        fo << x;
      }
      for (let y : r) return i << y.all();
      return πhr{}; })

    .def_ppost("G<", [](πi &i, πP<Γa<Γ>> g, ηic &x)
      { A(i.t(), "π G< with no τe");
        ξi r;
        {
          let [fo, fi] = i.t()->pipe();
          let [bo, bi] = i.t()->pipe();
          let s = Ξ{*i.t()}.push(ξdc{fi, bo});
          (*g).x(s);
          r = bi;
          fo << x;
        }
        for (let y : r) return i << y.all();
        return πhr{}; })
    ;
}


}

#include "end.hh"
