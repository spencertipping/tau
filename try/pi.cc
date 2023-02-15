#include <cstring>
#include <iostream>
#include <string>

#include "../tau.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;


πfn parse(St source)
{
  let r = (*φπ())(φc_{source});
  A(r.is_a(),
    "parse failure at offset " << r.j << ": " << r.e << " " << r.t);
  return r.y.value();
}


bool run(πfn f)
{
  πi i;
  let r = f.run(i);
  cout << i << endl;
  return r;
}


bool run(St source)
{
  return run(parse(source));
}


int main(int argc, char **argv)
{
  τassert_begin;
  A(argc > 1, "usage: " << argv[0] << " command");
  if (!strcmp(argv[1], "parse"))
  { cout << parse(argv[2]) << endl;
    return 0; }

  if (!strcmp(argv[1], "run")) ++argv, --argc;

  return run(St{argv[1]}) ? 0 : 1;
  τassert_end;
}



#include "../tau/end.hh"
