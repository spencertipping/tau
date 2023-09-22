#include <fstream>

#include "../sigma.hh"
#include "../tau/begin.hh"

using namespace σ;
using namespace std;


τ::St read_file(τ::St filename)
{
  ifstream f(filename);
  if (!f) throw τ::St("could not open file");
  return τ::St{istreambuf_iterator<char>(f), istreambuf_iterator<char>()};
}


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

  {
    // Test for a specific failure case involving duplicated π args
    using namespace τ;
    g.pi().def_ppost("##TEST", [](πi &i, πse<St> s1, πse<St> s2, πhr lhs)
      { std::cout << "TEST: " << i[lhs] << " " << s1 << " " << s2 << std::endl;
        return lhs; });

    g.pi().def_ppost("##iTEST", [](πi &i, πse<i64> s1, πse<St> s2, πhr lhs)
      { std::cout << "iTEST: " << i[lhs] << " " << s1 << " " << s2 << std::endl;
        return lhs; });
  }

  τ::St tau = argv[1];
  if (access(argv[1], F_OK) != -1) tau = read_file(tau);

  auto r = g.parse(tau);
  if (r.is_f())
  {
    cerr << "parse error at " << r.j << ": " << r.p().name() << endl;
    cerr << "here -> " << tau.substr(r.j) << endl;
    return 1;
  }

  if (debug) cerr << "Γ = " << r.r() << endl;

  let has_out = tau.find(">F1") != τ::St::npos;
  let p       = has_out ? r.r() : (r.r() | g.parse("p>Snx;>F1").r());
  p(Ξ{t}.push());
  t.go();
  return 0;
}

#include "../tau/end.hh"
