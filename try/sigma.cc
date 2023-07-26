#include "../sigma.hh"
#include "../tau/begin.hh"

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

  {
    using namespace τ;
    g.pi().def_ppost("##TEST", [](πi &i, πse<St> s1, πse<St> s2, πhr lhs)
      { std::cout << "TEST: " << i[lhs] << " " << s1 << " " << s2 << std::endl;
        return lhs; });

    g.pi().def_ppost("##iTEST", [](πi &i, πse<i64> s1, πse<St> s2, πhr lhs)
      { std::cout << "iTEST: " << i[lhs] << " " << s1 << " " << s2 << std::endl;
        return lhs; });

    {
      // NOTE: this works, so the problem is on the π side, not the φ side.
      auto p = φauto(g, [](St a, φig, St b) { return a + ":" + b; });
      auto r = p("\"123\" \"456\"");
      A(r.is_a(), "φauto failed parse: " << r.p().name());
      A(r.r() == "123:456", "φauto failed result: " << r.r());
    }

    {
      auto p = φs("foo", g.p(null<St>()), g.p(null<φig>()), g.p(null<St>()));
      auto r = p("\"123\" \"456\"");
      A(r.is_a(), "φauto failed parse: " << r.p().name());

      auto [a, _, b] = r.r();
      auto aa = a;
      auto bb = b;
      A(a == "123", "φauto failed result: " << aa << " != 123");
      A(b == "456", "φauto failed result: " << bb << " != 456");
    }
  }

  try
  {
    auto r = g.parse(τ::St{argv[1]});
    if (r.is_f())
    {
      cerr << "parse error at " << r.j << ": " << r.p().name() << endl;
      cerr << "here -> " << τ::St{argv[1]}.substr(r.j) << endl;
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

#include "../tau/end.hh"
