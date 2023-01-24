//#define τdebug_i9st 1

#include "../tau.hh"

#include "../tau/begin.hh"

using namespace std;
using namespace τ;
using namespace τ::ξ;


int hello(int argc, char **argv)
{
  Φ f;
  tcp_server(f, argc ? atoi(argv[0]) : 3000)
    | io<i9, fd_t>(f, [&](i9 x)
      { let fd = Sc<u9_scoped<u9_Φ, fd_t>>(x[0]).x;
        fd_io(f, fd, ϝ::ξΘ) | http1(f) | stream_out(f, cout);
        return fd; })
    | stream_out(f, cout);

  cout << "http listening" << endl;
  f.go();
  return 0;
}


int main(int argc, char **argv)
{
  τassert_begin;
  if (!strcmp(argv[1], "hello")) return hello(argc - 2, argv + 2);

  cerr << "unknown command " << argv[1] << endl;
  τassert_end;
  return 1;
}


#include "../tau/end.hh"
