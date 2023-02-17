#include <filesystem>
#include <fstream>
#include <iostream>

#include "../tau.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;

namespace fs = std::filesystem;


void run_gamma(St source)
{
  let r = (*φE(φγ()))(φc_(source));
  A(r.is_a(),
    "failed to parse " << source << " at location "
    << r.j << ": " << r.e << " " << r.t << std::endl
    << "problem is here: →|" << source.substr(r.j));

  πi i{Sp<πfn>(new πfn(*r.y))};
  A(i.run(0), "failed to run " << *r.y << "; i = " << i);

  (i.dpop().as_γ() | γostream(std::cout))(τe{}).go();
}


St lit_or_file(chc *a)
{
  if (fs::exists(fs::path(a)))
  {
    ifstream ifs("myfile.txt");
    St s{istreambuf_iterator<char>(ifs),
         istreambuf_iterator<char>()};
    return s;
  }
  else
    return St{a};
}


int main(int argc, char **argv)
{
  τassert_begin;
  A(argc > 1, "usage: " << argv[0] << " filename-or-source");
  run_gamma(lit_or_file(argv[1]));
  return 0;
  τassert_end;
}



#include "../tau/end.hh"
