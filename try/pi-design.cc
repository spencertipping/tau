#include <ostream>
#define tau_debug 1
//#define tau_debug_i9st 1
#include "../tau.hh"
#include "../tau/debug.hh"

#if τdefines_Φ


using namespace τ;
using namespace τ::ξ;
using namespace std;


#include "../tau/begin.hh"


void try_split()
{
  Φ f;
  fd_in(f, open("/tmp/1m-jsons", O_RDONLY))
    | split_chr(f, '\n')
    | take(f, 100)
    | fd_out(f, 1);
    //| stream_out(f, cout);
  f.go();
}


void bench_json_filter()
{
  Φ f;
  cout << "bench json TODO" << endl;
}


int main(int argc, char **argv)
{
  try_split();
  bench_json_filter();
  return 0;
}

#include "../tau/end.hh"


#else

int main(int argc, char **argv)
{
  std::cout << "no π on this platform" << std::endl;
  return 0;
}

#endif
