#define tau_debug 1
//#define tau_debug_i9st 1
#include "../tau.hh"
#include "../tau/debug.hh"


using namespace tau;
using namespace std;


#include "../tau/begin.hh"


typedef γ<> G;


G &γn(G &g, uN l)
{
  g.λc([&]() {
    for (uN i = 0; i < l; ++i) if (!(g.ο() << o9(i))) return 1;
    return 0;
  });
  return g;
}


G &γi(G &g)
{
  g.λc([&]() {
    for (let i : g.ι()) if (!(g.ο() << o9(Sc<uN>(i) + 1))) return 1;
    return 0;
  });
  return g;
}


G &γo(G &g)
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
  G f(l);
  G g(l);
  G h(l);

  γn(f, 100) | γi(g) | γo(h);
  l.go();
}


int main(int argc, char **argv)
{
  try_simple();
  return 0;
}

#include "../tau/end.hh"
