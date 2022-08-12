#include <thread>
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


γ &plus(γ &g, i64 n)
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
        else g.ψw("stdout"y, u9{Sc<i64>(v) + n});
      }
    }
    g.ψw("stdin"y, ω);
    g.ψw("stdout"y, ω);
    return 0;
  });
  return g;
}


γ &is_mod(γ &g, i64 n)
{
  g.ψw("stdin"y, α);
  g.ψw("stdout"y, α);
  g.λc("main"y, [&, n]() {
    while (g.ψrw("stdin"y))
    {
      let v = g.ψr("stdin"y);
      if (!v.is_greek())
      {
        let i = Sc<i64>(v);
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
      let v = g.ψr("stdin"y);
      if (!v.is_greek())
      {
        g.Θw(t);
        if (!g.ψww("stdout"y)) break;
        else g.ψw("stdout"y, v);
      }
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


γ &multisum(γ &g)
{
  auto *vs    = new S<u9>;
  i64  *total = new i64;

  *total = 0;

  g.ψw("connect"y, α);
  g.λc("main"y, [&, vs, total]() {
    cout << "multisum main started" << endl;

    while (g.ψrw("connect"y))
    {
      cout << "multisum connection" << endl;
      auto p = g.gs();
      g.ψm("connect"y, p);
      g.ψw("connect"y, α);

      cout << "multisum: moved to " << p << endl;

      vs->emplace(p);
      cout << "multisum: creating socket process " << p << endl;
      g.λc(p, [&, p = std::move(p), vs, total]() {
        cout << "multisum " << p << ": started" << endl;
        while (g.ψrw(p))
        {
          let v = g.ψr(p);
          cout << "multisum " << p << ": received " << v << endl;
          if (!v.is_greek())
          {
            *total += Sc<i64>(v);
            for (let &u : *vs)
              if (g.ψww(u)) g.ψw(u, u9{*total});
          }
        }
        cout << "multisum " << p << ": done" << endl;
        vs->erase(p);
        g.ψw(p, ω);
        return 0;
      });
    }

    cout << "multisum: done" << endl;
    g.ψw("connect"y, ω);

    return 0;
  });

  return g;
}


γ &nprint(Γ &G, γ &g, γ &s, i64 n_)
{
  g.ψw("socket"y, α);

  cout << "setting up writer for " << g.i << endl;
  g.λc("writer"y, [&, n_]() {
    cout << "socket connection for " << g.i << endl;
    G.φc(g, "socket"y, s, "connect"y, 64);
    cout << "setting up reader for " << g.i << endl;

    g.λc("reader"y, [&]() {
      while (g.ψrw("socket"y))
        cout << "client " << g.i << " received " << g.ψr("socket"y) << endl;
      g.ψw("socket"y, ω);
      return 0;
    });

    for (i64 i = 0; i < n_; ++i)
    {
      cout << g.Θi() << ": >>> " << i << endl;
      if (!g.ψww("socket"y)) break;
      else g.ψw("socket"y, u9{i});
    }
    return 0;
  });

  return g;
}


void try_nested()
{
  Γ g;
  γ &a = g.γc();

  a.λc("outer"y, [&]() {
    cout << "outer fn" << endl;
    a.λc("inner"y, [&]() {
      cout << "inner fn" << endl;
      return 0;
    });
    cout << "outer fn returning" << endl;
    return 0;
  });

  g.go();
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
  for (Θp t; (t = g.go()) != never();)
    std::this_thread::sleep_until(t);

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
    &b = sleep(g.γc(), 1ms),
    &c = out(g.γc());

  cout << "connecting stdout <-> stdin" << endl;
  g.φc(a, "stdout"y, b, "stdin"y, 64);
  g.φc(b, "stdout"y, c, "stdin"y, 64);

  cout << "running gamma" << endl;

  for (Θp t; (t = g.go()) != never();)
  {
    cout << "sleeping until t+" << now() - t << endl;
    std::this_thread::sleep_until(t);
  }

  cout << "timings:" << endl;
  cout << "  a = " << a.γΘ() << endl;
  cout << "  b = " << b.γΘ() << endl;
  cout << "  c = " << c.γΘ() << endl;

  cout << g << endl;
}


void try_server()
{
  Γ g;
  γ &m = multisum(g.γc()),
    &a = nprint(g, g.γc(), m, 10),
    &b = nprint(g, g.γc(), m, 20);

  cout << "running gamma" << endl;
  for (Θp t; (t = g.go()) != never();)
  {
    cout << "sleeping until t+" << now() - t << endl;
    std::this_thread::sleep_until(t);
  }

  cout << "done" << endl;
}


int main()
{
  cout << "try_nested" << endl; try_nested();
  cout << "try_gamma"  << endl; try_gamma();
  cout << "try_sleep"  << endl; try_sleep();
  cout << "try_server" << endl; try_server();
  return 0;
}


}

#include "../tau/module/end.hh"
