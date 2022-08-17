#include <thread>
#include <unistd.h>


#ifndef QUICK
# define QUICK 0
#endif


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
  g.ψw("stdout"_q, α);
  g.λc("main"_l, [&, n_]() {
    for (i64 i = 0; i < n_; ++i)
    {
      cout << g.Θi() << ": >>> " << i << endl;
      if (!g.ψww("stdout"_q)) break;
      else g.ψw("stdout"_q, u9{i});
    }
    g.ψw("stdout"_q, ω);
    return 0;
  });
  return g;
}


γ &plus(γ &g, i64 n)
{
  g.ψw("stdin"_q, α);
  g.ψw("stdout"_q, α);
  g.λc("main"_l, [&, n]() {
    while (g.ψrw("stdin"_q))
    {
      let v = g.ψr("stdin"_q);
      if (!v.is_greek())
      {
        if (!g.ψww("stdout"_q)) break;
        else g.ψw("stdout"_q, u9{Sc<i64>(v) + n});
      }
    }
    g.ψw("stdin"_q, ω);
    g.ψw("stdout"_q, ω);
    return 0;
  });
  return g;
}


γ &is_mod(γ &g, i64 n)
{
  g.ψw("stdin"_q, α);
  g.ψw("stdout"_q, α);
  g.λc("main"_q, [&, n]() {
    while (g.ψrw("stdin"_q))
    {
      let v = g.ψr("stdin"_q);
      if (!v.is_greek())
      {
        let i = Sc<i64>(v);
        if (!(i % n))
          if (!g.ψww("stdout"_q)) break;
          else g.ψw("stdout"_q, u9{i});
      }
    }
    g.ψw("stdin"_q, ω);
    g.ψw("stdout"_q, ω);
    return 0;
  });
  return g;
}


γ &sleep(γ &g, ΔΘ t)
{
  g.ψw("stdin"_q, α);
  g.ψw("stdout"_q, α);
  g.λc("main"_l, [&, t]() {
    while (g.ψrw("stdin"_q))
    {
      let v = g.ψr("stdin"_q);
      if (!v.is_greek())
      {
        g.Θw(t);
        if (!g.ψww("stdout"_q)) break;
        else g.ψw("stdout"_q, v);
      }
    }
    g.ψw("stdin"_q, ω);
    g.ψw("stdout"_q, ω);
    return 0;
  });
  return g;
}


γ &out(γ &g)
{
  g.ψw("stdin"_q, α);
  g.λc("main"_l, [&]() {
    while (g.ψrw("stdin"_q))
      cout << g.Θi() << ": " << g.ψr("stdin"_q) << endl;
    g.ψw("stdin"_q, ω);
    return 0;
  });
  return g;
}


γ &multisum(γ &g)
{
  auto *vs    = new S<γsi>;
  i64  *total = new i64;

  *total = 0;

  g.ψw("connect"_l, α);
  g.λc("main"_l, [&, vs, total]() {
    cout << "multisum main started" << endl;

    while (g.ψφw("connect"_l))
    {
      cout << "multisum connection" << endl;
      auto p = g.gs();
      g.ψm("connect"_l, p);

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
    g.ψw("connect"_l, ω);

    return 0;
  });

  return g;
}


γ &nprint(Γ &G, γ &g, γ &s, i64 n_)
{
  g.ψw("socket"_l, α);

  cout << "setting up writer for " << g.i << endl;
  g.λc("writer"_l, [&, n_]() {
    cout << "socket connection for " << g.i << endl;
    if (!s.ψxw("connect"_l))
    {
      cout << "server closed connection before we could connect" << endl;
      _exit(1);
    }

    // TODO: add φc to γ
    G.φc(g, "socket"_l, s, "connect"_l, 64);
    cout << "setting up reader for " << g.i << endl;

    g.λc("reader"_l, [&]() {
      i64 l = 0;
      while (g.ψrw("socket"_l))
      {
        let v = g.ψr("socket"_l);
        cout << "client " << g.i << " received " << v << endl;
        if (!v.is_greek()) l = Sc<i64>(v);
      }
      cout << "client " << g.i << " returning " << l << endl;
      return l;
    });

    for (i64 i = 0; i < n_; ++i)
    {
      cout << g.Θi() << ": >>> " << i << endl;
      if (!g.ψww("socket"_l)) cout << "failed to write " << i << endl;
      else g.ψw("socket"_l, u9{i});
    }

    return 0;
  });

  return g;
}


void try_nested()
{
  Γ g;
  γ &a = g.γc();

  a.λc("outer"_q, [&]() {
    cout << "outer fn" << endl;
    a.λc("inner"_q, [&]() {
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

  cout << "stdin_q  = " << "stdin"_q  << endl;
  cout << "stdout_q = " << "stdout"_q << endl;
  cout << "main_l   = " << "main"_q   << endl;

  cout << "connecting stdout <-> stdin" << endl;
  g.φc(a, "stdout"_q, b, "stdin"_q, 64);
  g.φc(b, "stdout"_q, c, "stdin"_q, 64);
  g.φc(c, "stdout"_q, d, "stdin"_q, 64);

  cout << "running gamma" << endl;
  for (Θp t; (t = g.go()) != never();)
    std::this_thread::sleep_until(t);

  cout << "done; exit state is " << d.λw("main"_l) << endl;
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
    &b = sleep(g.γc(), QUICK ? 500ns : 1ms),
    &c = out(g.γc());

  cout << "connecting stdout <-> stdin" << endl;
  g.φc(a, "stdout"_q, b, "stdin"_q, 64);
  g.φc(b, "stdout"_q, c, "stdin"_q, 64);

  cout << "running gamma" << endl;

  for (Θp t; (t = g.go()) != never();)
  {
    cout << "sleeping until " << t << endl;
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
    cout << "sleeping until " << t << endl;
    std::this_thread::sleep_until(t);
  }

  a.ψw("socket"_l, ω);
  b.ψw("socket"_l, ω);
  for (Θp t; (t = g.go()) != never();)
  {
    cout << "sleeping until " << t << endl;
    std::this_thread::sleep_until(t);
  }

  let ar = a.λw("reader"_l);
  let br = b.λw("reader"_l);

  cout << "ar = " << ar << endl;
  cout << "br = " << br << endl;

  assert(ar == 235);
  assert(br == 235);

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
