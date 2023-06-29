#include "../sigma.hh"

using namespace σ;
using namespace std;


int main(int argc, char *argv[])
{
  bool debug = false;

  if (argc < 2)
  {
    cerr << "Usage: " << argv[0] << " Γ" << endl;
    return 1;
  }
  else if (argc == 3)
  {
    debug = true;
    ++argv, --argc;
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

    if (debug) cerr << "Γ = " << r.r() << endl;

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
