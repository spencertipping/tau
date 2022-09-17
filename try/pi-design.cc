#include "../dep/rapidjson-h/document.h"


#include <ostream>
#define tau_debug 1
//#define tau_debug_i9st 1
#include "../tau.hh"
#include "../tau/debug.hh"

#if τdefines_Φ


using namespace τ;
using namespace τ::ξ;
using namespace std;
using namespace rapidjson;


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
  fd_in(f, open("/tmp/1m-jsons", O_RDONLY))
    | split_chr(f, '\n')
    | *new ϝ(f, "json", ϝ::ξι, [](ϝ &f)
      { Document d;
        for (let x : f)
        { d.Parse(Rc<chc*>(x.data()), x.size());
          if (!(f.β() << o9t(d["id"].GetString(),
                             d["author"].GetString())))
            break; }})
    | *new ϝ(f, "filter", ϝ::ξι, [](ϝ &f)
      { for (let x : f)
          if (x[1] != "[deleted]")
            if (!(f.β() << x)) break; })
    | stream_out(f, cout);
  f.go();
}


int main(int argc, char **argv)
{
  //try_split();
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
