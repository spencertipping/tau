#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void Γshared_ψ(Γφ &g)
{
  g .def_p0(">_", [](            ξi i) { for (let x : i) (void) x; })

    .def_p1("<_", [](       ψ q, ξo o) { τe &t = q.t(); while (1) t.dt(now() + 1s); })
    .def_p1("i",  [](ηm  x,      ξo o) { o.r(x.lsize()) << x.all(); })
    .def_p1("I",  [](ηm  x,      ξo o) { while (o) o.r(x.lsize()) << x.all(); })
    .def_p1("n",  [](i64 x,      ξo o) { for (i64 i = 0; i < x; ++i) o.r(12) << i; })

    .def_p2(",",   [](          ξi i, ξo o) {})
    .def_p2("-",   [](          ξi i, ξo o) { for (let x : i) o << x; })
    .def_p2("k",   [](          ξi i, ξo o) { for (let x : i) o << x, o.r(2) << ηsig::τ; })
    .def_p2("K",   [](          ξi i, ξo o) { for (let x : i) if (!x.is_sig()) o << x; })
    .def_p2("N",   [](          ξi i, ξo o) { ηl(i, o, [&](ηi x) { for (i64 n = 0; n < x.i(); ++n) o.r(12) << n; o.r() << ηsig::τ; }); })
    .def_p2("M?",  [](          ξi i, ξo o) { for (let x : i) std::cout <<              x << std::endl, o << x; })
    .def_p2("M??", [](St n,     ξi i, ξo o) { for (let x : i) std::cout << n << ": " << x << std::endl, o << x; })
    .def_p2("p",   [](πt<π1> f, ξi i, ξo o) { πi p(i, o); for (let x : i) { p.push(p << x.all()); p.def_x(); f.x(p); if (let r = p.pop()) o << p[r]; p.clear_xy(); } })
    .def_p2("r",   [](πt<π1> f, ξi i, ξo o) { πi p(i, o); for (let x : i) { p.push(p << x.all()); p.def_x(); f.x(p); if (p.ypop().cb()) o << x; p.clear_xy(); } })
    .def_p2("α.",  [](          ξi i, ξo o) { for (let x : i) o.r(x.lsize() + 2) << ηsig::α << x; })
    .def_p2("κ.",  [](          ξi i, ξo o) { for (let x : i) o.r(x.lsize() + 2) << ηsig::κ << x; })
    .def_p2("ι.",  [](          ξi i, ξo o) { for (let x : i) o.r(x.lsize() + 2) << ηsig::ι << x; })
    .def_p2("ω.",  [](          ξi i, ξo o) { for (let x : i) o.r(x.lsize() + 2) << ηsig::ω << x; })
    .def_p2("τ.",  [](          ξi i, ξo o) { for (let x : i) o.r(x.lsize() + 2) << ηsig::τ << x; })

    .def_c2(";",   [](Γ g, ψ q, ξi i, ξo o) { g(Ξ{q.t()}.push(ξd{i, o})); })
    ;
}


}

#include "end.hh"
