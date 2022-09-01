#define tau_debug 1
//#define tau_debug_i9st 1
#include "../tau.hh"
#include "../tau/debug.hh"


using namespace tau;
using namespace std;


#if tau_defines_γ


#include "../tau/begin.hh"


γ &γn(γ &g, uN l)
{
  g.λc([&, l]() {
    for (uN i = 0; i < l; ++i)
      A(g.ο() << i, "n rejected " << i);
    g.ο().ω();
    return 0;
  });
  return g;
}


γ &γi(γ &g)
{
  g.λc([&]() {
    u64 t = 0;
    uN  n = 0;
    for (let i : g.ι())
    {
      let x = Sc<uN>(i);
      t += x;
      A(g.ο() << o9t(x + 1, t), "i rejected " << x);
      if (!(++n % 100000))
        g.δ() << o9t("total", t);
    }
    g.ο().ω();
    g.δ().ω();
    return 0;
  });
  return g;
}


γ &γf(γ &g, uN e)
{
  g.λc([&, e]() {
    uN i = 0;
    for (let x : g.ι())
      if (!(++i % e))
        A(g.ο() << x, "f rejected " << x);
    g.ο().ω();
    return 0;
  });
  return g;
}


γ &γo(γ &g)
{
  g.λc([&]() {
    for (let i : g.ι()) cout << i << endl;
    return 0;
  });
  return g;
}


void try_simple()
{
  Λ l;
  Φ f(l);
  γ a(f);
  γ b(f);
  γ c(f);
  γ d(f);
  γ e(f);

  γn(a, 1000000) | γi(b) | γf(c, 50000) | γo(d);
  b & γo(e);
  f.go();

  cout << a << endl;
  cout << b << endl;
  cout << c << endl;
  cout << d << endl;
  cout << e << endl;
  cout << f << endl;

  /*
  cout << "a IOE = " << a.ι().f.im << " " << a.ο().f.om << " " << a.δ().f.om << endl;
  cout << "b IOE = " << b.ι().f.im << " " << b.ο().f.om << " " << b.δ().f.om << endl;
  cout << "c IOE = " << c.ι().f.im << " " << c.ο().f.om << " " << c.δ().f.om << endl;
  cout << "d IOE = " << d.ι().f.im << " " << d.ο().f.om << " " << d.δ().f.om << endl;
  cout << "e IOE = " << e.ι().f.im << " " << e.ο().f.om << " " << e.δ().f.om << endl;
  */
}


int main(int argc, char **argv)
{
  try_simple();
  return 0;
}

#include "../tau/end.hh"


#else

int main()
{
  cout << "no γ support on this platform" << endl;
  return 1;
}

#endif
