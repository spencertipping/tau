#include "platform.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


Γφ σΓ()
{
  Γφ r;
  Γshared(r);
  Γnative(r);
  πshared(r.pi());
  πnative(r.pi());
  return r;
}


void Γshared(Γφ &g)
{
  g .def_g("=",  [](Ξc &x) { return x.push(); })
    .def_g("_",  [](Ξc &x) { return x.drop(); })
    .def_g(":",  [](φident n, Γ g, Ξc &x) { return x.gs(n.x, g); })
    .def_g("$",  [](φident n,      Ξc &x) { return Γ{x.gg(n.x)}(x); })
    .def_g(":/", [](φident n,      Ξc &x) { let [d, y] = x.pop(); return y.vs(n.x, d); })
    .def_g("$/", [](φident n,      Ξc &x) { return x.push(x.vg(n.x)); })
    .def_g("$$", [](               Ξc &x) { let [a, y] = x.pop(); let [b, z] = y.pop(); return z.push(a).push(b); })

    .def_p0("F", [](            ξi i) { for (let x : i) (void) x; })

    .def_p1("f", [](       ψ q, ξo o) { τe &t = q.t(); while (1) t.dt(now() + 1s); })
    .def_p1("i", [](ηm  x,      ξo o) { o.r(x.lsize()) << x.all(); })
    .def_p1("I", [](ηm  x,      ξo o) { while (o) o.r(x.lsize()) << x.all(); })
    .def_p1("n", [](i64 x,      ξo o) { for (i64 i = 0; i < x; ++i) o.r() << i; })

    .def_p2("-",   [](          ξi i, ξo o) { for (let x : i) o << x; })
    .def_p2("k",   [](          ξi i, ξo o) { for (let x : i) o << x, o.r(2) << ηsig::τ; })
    .def_p2("K",   [](          ξi i, ξo o) { for (let x : i) if (!x.is_sig()) o << x; })
    .def_p2("N",   [](          ξi i, ξo o) { ηl(i, o, [&](ηi x) { for (i64 n = 0; n < x.i(); ++n) o.r(12) << n; }); })
    .def_p2("M?",  [](          ξi i, ξo o) { for (let x : i) std::cout <<              x << std::endl, o << x; })
    .def_p2("M??", [](St n,     ξi i, ξo o) { for (let x : i) std::cout << n << ": " << x << std::endl, o << x; })
    .def_p2("p",   [](πt<π1> f, ξi i, ξo o) { πi p(i, o); for (let x : i) { p.push(p << x.all()); p.def_x(); f.x(p); o << p.ypop(); p.clear_xy(); } })
    .def_p2("r",   [](πt<π1> f, ξi i, ξo o) { πi p(i, o); for (let x : i) { p.push(p << x.all()); p.def_x(); f.x(p); if (p.ypop().cb()) o << x; p.clear_xy(); } })
    ;
}


void πshared(πφ &p)
{
  p .def_sa([](f64 x)    { return πk(x); })
    .def_sa([](i64 x)    { return πk(x); })
    .def_sa([](ηname n)  { return πk(n); })
    .def_sa([](φaL<'x'>) { return π1{"x", [](πi &i) { i.push(i.x()); }}; })
    .def_sa([](φaL<'y'>) { return π1{"y", [](πi &i) { i.push(i.y()); }}; })

    .def_sa([](φaL<'$'>, φident n)
      { return π1{"$" + n.x, [=](πi &i)
        { let x = i.mg(n.x);
          i.push(i.i(x, i[x].one())); }}; })

    .def_pa([](φaL<'@'>, φident n)
      { return π1{"@" + n.x, [=](πi &i) { i.push(i.mg(n.x)); }}; })
    .def_pa([](φident n, φaL<'='>, πpa<π1> y)
      { return π1{n.x + "=", [=](πi &i) { y.x(i); i.ms(n.x, i.peek()); }}; })

    .def_sp("+",  [](πse<i64> y, i64 x) { return x + y.x; })
    .def_sp("-",  [](πse<i64> y, i64 x) { return x - y.x; })
    .def_sp("*",  [](πse<i64> y, i64 x) { return x * y.x; })
    .def_sp("/",  [](πse<i64> y, i64 x) { return x / y.x; })
    .def_sp("%",  [](πse<i64> y, i64 x) { return x % y.x; })
    .def_sp("&",  [](πse<i64> y, i64 x) { return x & y.x; })
    .def_sp("|",  [](πse<i64> y, i64 x) { return x | y.x; })
    .def_sp("^",  [](πse<i64> y, i64 x) { return x ^ y.x; })

    .def_ssp("~", [](i64 x) { return ~x; })
    .def_ssp("_", [](i64 x) { return -x; })

    .def_sp("+.", [](πse<f64> y, f64 x) { return x + y.x; })
    .def_sp("-.", [](πse<f64> y, f64 x) { return x - y.x; })
    .def_sp("*.", [](πse<f64> y, f64 x) { return x * y.x; })
    .def_sp("/.", [](πse<f64> y, f64 x) { return x / y.x; })
    .def_sp("%.", [](πse<f64> y, f64 x) { return fmod(x, y.x); })

    .def_ssp("_.", [](f64 x) { return -x; })

    .def_ssp("S.", [](f64 x) { return sin(x); })
    .def_ssp("C.", [](f64 x) { return cos(x); })
    .def_ssp("T.", [](f64 x) { return tan(x); })
    .def_ssp("L.", [](f64 x) { return log(x); })
    .def_ssp("E.", [](f64 x) { return exp(x); })

    .def_spp("@", [](ηic &x) { return x.η().all(); })

    .def_sp(">",  [](πse<ηi> const &y, ηic &x) { return x > y.x; })
    .def_sp(">=", [](πse<ηi> const &y, ηic &x) { return x >= y.x; })
    .def_sp("<",  [](πse<ηi> const &y, ηic &x) { return x < y.x; })
    .def_sp("<=", [](πse<ηi> const &y, ηic &x) { return x <= y.x; })
    .def_sp("==", [](πse<ηi> const &y, ηic &x) { return (x <=> y.x) == PO::equivalent; })
    .def_sp("!=", [](πse<ηi> const &y, ηic &x) { return (x <=> y.x) != PO::equivalent; })

    .def_psp(":", [](πi &i, πP<ηname> const &n, πhr const &x)
      { let r = i[x].at(n.x);
        std::cout << "|r|: " << r.size() << std::endl;
        return r.empty() ? i << ηsig::ω : i.i(x, ηi{r}.one()); })

    .def_sp("?", [](πi &i, πse<π1> const &y, φaL<':'>, πse<π1> const &z, bool c)
      { return (c ? y.x(i) : z.x(i)).pop(); })

    .def_pp("`", [](πi &i, πpe<π1> const &y, πhr const&)
      { return y.x(i).pop(); })

    .def_sp(">s", [](ηic &x) { return (Ss{} << x << "\n").str(); })
    ;
}


}

#include "end.hh"
