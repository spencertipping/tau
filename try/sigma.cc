#include "../sigma.hh"

using namespace σ;
using namespace std;


int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cerr << "Usage: " << argv[0] << " Γ" << endl;
    return 1;
  }

  τe t;
  auto g = σΓ();
  try
  {
    auto r = g.parse(τ::St{argv[1]});
    if (r.is_f())
    {
      cerr << "parse error at " << r.j << ": " << r.p().name() << endl;
      return 1;
    }

    r.r()(Ξ{t}.push());
    t.go();
    return 0;
  }
  catch (τ::St s)
  {
    cerr << "runtime error: " << s << endl;
    return 1;
  }
}
