#include <filesystem>
#include <fstream>
#include <iostream>

#include "../tau.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;

namespace fs = std::filesystem;


void run_gamma(St source, bool silent = false)
{
  let r = (*φE(φΓ()))(φc_(source));
  A(r.is_a(),
    "failed to parse " << source << " at location "
    << r.j << ": " << r.e << " " << r.t << std::endl
    << "problem is here: →|" << source.substr(r.j));

  πi i{Sp<πfn>(new πfn(*r.y))};
  A(i.run(0), "failed to run " << *r.y << "; i = " << i);

  if (silent)
    i.dpop().as_Γ()(τe{}).go();
  else
    (i.dpop().as_Γ() | Γostream(std::cout))(τe{}).go();
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
  if (St{argv[1]} == "nrun") run_gamma(lit_or_file(argv[2]), true);
  else                       run_gamma(lit_or_file(argv[1]));
  return 0;
  τassert_end;
}



#include "../tau/end.hh"
