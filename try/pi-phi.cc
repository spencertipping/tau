#include "../tau.hh"
#include "../tau/begin.hh"

using namespace std;
using namespace τ;


πv eval_pi_core_expr(St e)
{
  τe t;
  πi i{t};
  let p = φE(πφ());
  let x = p(e);
  A(x.is_a(), "parse failed: " << x.e << " beginning here: → |" << e.substr(x.j));
  return (*x.y)(i);
}


int main(int argc, char **argv)
{
  A(argc >= 2, "usage: " << argv[0] << " 'pi expr'");
  for (int i = 1; i < argc; ++i)
    cout << eval_pi_core_expr(argv[i]) << endl;
  return 0;
}


#include "../tau/end.hh"
