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

  try
  {
    auto r = τ::φE(φΓ())(argv[1]);
    if (r.is_f())
    {
      cerr << "parse error at " << r.j << ": " << r.p().name() << endl;
      return 1;
    }

    cout << r.r() << endl;

    τ::τe t;
    r.r()(τ::Ξ{t});
    t.go();
    return 0;
  }
  catch (std::string s)
  {
    cerr << "runtime error: " << s << endl;
    return 1;
  }
}
