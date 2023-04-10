#include "../tau.hh"

using namespace std;
using namespace τ;

#include "../tau/begin.hh"


void try_parsers()
{
  let a = φl("a", 3);
  let b = φl("b", 4.0);
  let c = φl("c", "hi there");
  let d = φCJK();
  let p = φs(a, b, c, d);
  cout << p.name() << endl;

  let r = p("abc八次不");
  if (r.is_f()) cout << "parse failed: " << r.e << ", " << r.t << endl;
  else
  {
    let [a_, b_, c_, d_] = *r.y;
    cout << a_ << ", " << b_ << ", " << c_ << ", " << d_ << endl;
  }
}


int main()
{
  try_parsers();
  return 0;
}


#include "../tau/end.hh"
