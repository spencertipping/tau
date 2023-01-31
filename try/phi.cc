#include <iostream>

#include "../tau.hh"
//#include "../tau/phi-ctor.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;


void try_simple()
{
  let p = φn(φa<int>(φl("foo", 1), φl("bar", 2)));
  let q = φparse(p, "foobarba");
  if (q.has_value())
    for (let x : *q)
      cout << "got a result: " << x << endl;
  else
    cout << "parse failed (error)" << endl;
}


int main(int argc, char **argv)
{
  τassert_begin;
  try_simple();
  return 0;
  τassert_end;
}



#include "../tau/end.hh"
