#include <unistd.h>


#include "../tau.hh"


#include "../tau/module/begin.hh"

namespace t::gamma_basics
{

using namespace std;
using namespace tau;
using namespace tau::flux;
using namespace tau::util;
using tau::operator<<;


γ &n(γ &g, i64 n_)
{
  g.ψw("stdout"y, α);
  g.λc("main"y, [&, n_]() {
    for (i64 i = 0; i < n_; ++i)
    {
      cout << g.Θi() << ": >>> " << i << endl;
      if (!g.ψww("stdout"y)) break;
      else g.ψw("stdout"y, u9{i});
    }
    g.ψw("stdout"y, ω);
    return 0;
  });
  return g;
}


γ &plus(γ &g, iN n)
{
  g.ψw("stdin"y, α);
  g.ψw("stdout"y, α);
  g.λc("main"y, [&, n]() {
    while (g.ψrw("stdin"y))
    {
      let v = g.ψr("stdin"y);
      if (!v.is_greek())
      {
        if (!g.ψww("stdout"y)) break;
        else g.ψw("stdout"y, u9{Sc<iN>(v) + n});
      }
    }
    g.ψw("stdin"y, ω);
    g.ψw("stdout"y, ω);
    return 0;
  });
  return g;
}


γ &is_mod(γ &g, iN n)
{
  g.ψw("stdin"y, α);
  g.ψw("stdout"y, α);
  g.λc("main"y, [&, n]() {
    while (g.ψrw("stdin"y))
    {
      let v = g.ψr("stdin"y);
      if (!v.is_greek())
      {
        let i = Sc<iN>(v);
        if (!(i % n))
          if (!g.ψww("stdout"y)) break;
          else g.ψw("stdout"y, u9{i});
      }
    }
    g.ψw("stdin"y, ω);
    g.ψw("stdout"y, ω);
    return 0;
  });
  return g;
}


γ &sleep(γ &g, ΔΘ t)
{
  g.ψw("stdin"y, α);
  g.ψw("stdout"y, α);
  g.λc("main"y, [&, t]() {
    while (g.ψrw("stdin"y))
    {
      g.Θw(t);
      if (!g.ψww("stdout"y)) break;
      else g.ψw("stdout"y, g.ψr("stdin"y));
    }
    g.ψw("stdin"y, ω);
    g.ψw("stdout"y, ω);
    return 0;
  });
  return g;
}


γ &out(γ &g)
{
  g.ψw("stdin"y, α);
  g.λc("main"y, [&]() {
    while (g.ψrw("stdin"y))
      cout << g.Θi() << ": " << g.ψr("stdin"y) << endl;
    g.ψw("stdin"y, ω);
    return 0;
  });
  return g;
}


void try_gamma()
{
  Γ g;
  γ &a = n(g.γc(), 100),
    &b = plus(g.γc(), 6),
    &c = is_mod(g.γc(), 5),
    &d = out(g.γc());

  cout << "connecting stdout <-> stdin" << endl;
  g.φc(a, "stdout"y, b, "stdin"y, 64);
  g.φc(b, "stdout"y, c, "stdin"y, 64);
  g.φc(c, "stdout"y, d, "stdin"y, 64);

  cout << "running gamma" << endl;
  g.go();
  cout << "done; exit state is " << d.λw("main"y) << endl;
  cout << "timings:" << endl;
  cout << "  a = " << a.γΘ() << endl;
  cout << "  b = " << b.γΘ() << endl;
  cout << "  c = " << c.γΘ() << endl;
  cout << "  d = " << d.γΘ() << endl;

  cout << g << endl;
}


void try_sleep()
{
  Γ g;
  γ &a = n(g.γc(), 100),
    &b = sleep(g.γc(), 100ns),
    &c = out(g.γc());

  cout << "connecting stdout <-> stdin" << endl;
  g.φc(a, "stdout"y, b, "stdin"y, 64);
  g.φc(b, "stdout"y, c, "stdin"y, 64);

  cout << "running gamma" << endl;
  g.go();
  cout << "timings:" << endl;
  cout << "  a = " << a.γΘ() << endl;
  cout << "  b = " << b.γΘ() << endl;
  cout << "  c = " << c.γΘ() << endl;

  cout << g << endl;
}


int main()
{
  cout << "try_gamma" << endl; try_gamma();
  cout << "try_sleep" << endl; try_sleep();
  return 0;
}


}

#include "../tau/module/end.hh"
