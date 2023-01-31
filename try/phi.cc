#include <iostream>

#include "../tau.hh"
//#include "../tau/phi-ctor.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;


void try_simple()
{
  let p = φa<int>(φl("foo", 1), φl("bar", 2));
  let q = φparse(p, "foo");
  cout << "parse result: " << q.value_or(-1) << endl;
}


int main(int argc, char **argv)
{
  τassert_begin;
  try_simple();
  return 0;
  τassert_end;
}



#include "../tau/end.hh"
