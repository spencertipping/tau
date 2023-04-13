#include "../tau.hh"
#include "../tau/begin.hh"

using namespace std;
using namespace τ;


static bool timed = false;


πv eval_pi_core_expr(St e)
{
  τe t;
  πi i{t};
  let p = φE(πφ());

  let t1 = now();
  let x = p(e);
  let t2 = now();
  A(x.is_a(), "parse failed at " << x.p().name() << " beginning here: → |" << e.substr(x.j));

  let r = x.r()(i);
  let t3 = now();

  if (timed)
    cout << "parse: " << t2 - t1 << "; eval: " << t3 - t2 << endl;

  return r;
}


int main(int argc, char **argv)
{
  A(argc >= 2, "usage: " << argv[0] << " 'pi expr'");
  int i = 1;
  if (St{argv[1]} == "--timed") timed = true, ++i;
  for (; i < argc; ++i)
    cout << eval_pi_core_expr(argv[i]) << endl;
  return 0;
}


#include "../tau/end.hh"
