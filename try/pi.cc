#include <cstring>
#include <iostream>
#include <string>

#include "../tau.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;


πfn parse(St source)
{
  let e = φE(φπ());
  let r = (*e)(φc_{source});
  A(r.is_a(),
    "parse failure at offset " << r.j << ": " << r.e << " " << r.t);
  return *r.y;
}


πfn &outify(πfn &f)
{
  return f << πinsn{"out", [](πi &i)
    { if (i.dpeek().is_η()) i.o() << i.dpop().as_η();
      else                  i.o() << η1o(i.dpop().as_γ().name()); }};
}


void run(πfn f)
{
  (γπ(outify(f)) | γostream(cout))(τe{}).go();
}


void debug(πfn f)
{
  outify(f);
  cout << f << endl;
  (γπ(f, "", [](πi &i, πinsn const &s)
    { cout << "about to run " << s << " on " << i << endl;
      return true; }) | γostream(cout))(τe{}).go();
}


int main(int argc, char **argv)
{
  τassert_begin;
  A(argc > 1, "usage: " << argv[0] << " command");
  if (!strcmp(argv[1], "parse")) { cout << parse(argv[2]) << endl; return 0; }
  if (!strcmp(argv[1], "debug")) { debug(parse(St{argv[2]}));      return 0; }

  if (!strcmp(argv[1], "run")) ++argv, --argc;

  run(parse(St{argv[1]}));
  return 0;
  τassert_end;
}



#include "../tau/end.hh"
