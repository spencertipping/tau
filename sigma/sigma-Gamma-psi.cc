#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void Γshared_ψ(Γφ &g)
{
  g .def_p0(">_", [](             ξi i) { for (let &x : i) (void) x; })

    .def_p1("<_", [](        ψ q, ξo o) { τe &t = q.t(); while (1) t.dt(now() + 1s); })
    .def_p1("i",  [](ηmc &x,      ξo o) { o.r(x.lsize()) << x.all(); })
    .def_p1("I",  [](ηmc &x,      ξo o) { while (o) o.r(x.lsize()) << x.all(); })
    .def_p1("n",  [](i64  x,      ξo o) { for (i64 i = 0; i < x; ++i) o.r(12) << i; })

    .def_p2(",",   [](                 ξi i, ξo o) {})
    .def_p2("-",   [](                 ξi i, ξo o) { for (let &x : i) o << x; })
    .def_p2("K",   [](                 ξi i, ξo o) { for (let &x : i) if (!x.is_sig()) o << x; })
    .def_p2("N",   [](                 ξi i, ξo o) { ηl(i, o, [&](ηic &x) { for (i64 n = 0; n < x.i(); ++n) o.r(12) << n; o.r() << ηsig::τ; }); })
    .def_p2("M?",  [](                 ξi i, ξo o) { for (let &x : i) std::cerr <<              x << std::endl, o << x; })
    .def_p2("M??", [](Stc &n,          ξi i, ξo o) { for (let &x : i) std::cerr << n << ": " << x << std::endl, o << x; })
    .def_p2("p",   [](πtc<π1> &f, ψ q, ξi i, ξo o) { πi p(q.t(), i, o); q.weaken(); for (let &x : i) { p.push(p << x.all()); p.def_x(); f.x(p); if (let r = p.pop()) o << p[r]; p.clear_xy(); } })
    .def_p2("r",   [](πtc<π1> &f, ψ q, ξi i, ξo o) { πi p(q.t(), i, o); q.weaken(); for (let &x : i) { p.push(p << x.all()); p.def_x(); f.x(p); if (p.ypop().cb()) o << x; p.clear_xy(); } })

    .def_p2("g",   [](ξi i, ξo o)
      { V<ηm> xs;
        let g = [&]()
          { std::sort(xs.begin(), xs.end(), [](ηmc &a, ηmc &b) { return a < b; });
            for (let &y : xs) o << y;
            xs.clear();
            o.r() << ηsig::τ; };
        for (let &x : i)
          if (x.is_τ()) g();
          else          xs.push_back(x);
        if (!xs.empty()) g(); })

    .def_c2(";",   [](Γ g, ψ q, ξi i, ξo o) { g(Ξ{q.t()}.push(ξd{i, o})); })
    ;
}


}

#include "end.hh"
