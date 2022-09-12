//#define tau_debug_i9st 1

#include "../tau.hh"

#if τdefines_γ


#include <cstring>


#include "io.hh"

#include "../tau/begin.hh"

using namespace std;


int nop_seq(int argc, char **argv)
{
  Φ f;
  for (let x : ι(atoi(argv[0]))) cout << f.dt() << ": " << x << endl;
  return 0;
}


int tau_seq(int argc, char **argv)
{
  Φ f;
  iota(f, atoi(argv[0])) | delay(f, 50ms) | stream_out(f, std::cout);
  f.go();
  return 0;
}


int tau_fseq(int argc, char **argv)
{
  Φ f;
  iota(f, atoi(argv[0])) | stream_out(f, std::cout);
  f.go();
  return 0;
}


int chat(int argc, char **argv)
{
  Φ f;
  auto &b = broadcast(f);
  tcp_server(f, argc ? atoi(argv[0]) : 3000)
    | io<i9, uN>(f, [&](i9 x)
      { let fd = Sc<u9_pidfd>(x[0]).fd;
        b | fd_io(f, fd);
        return fd; })
    | stream_out(f, cout);

  b & stream_out(f, cout);

  f.go();
  return 0;
}


int cat(int argc, char **argv)
{
  Φ f;
  fd_in(f, 0) | fd_out(f, 1);
  f.go();
  return 0;
}


int main(int argc, char **argv)
{
  if (!strcmp(argv[1], "nseq")) return nop_seq (argc - 2, argv + 2);
  if (!strcmp(argv[1], "tseq")) return tau_seq (argc - 2, argv + 2);
  if (!strcmp(argv[1], "fseq")) return tau_fseq(argc - 2, argv + 2);
  if (!strcmp(argv[1], "chat")) return chat    (argc - 2, argv + 2);
  if (!strcmp(argv[1], "cat"))  return cat     (argc - 2, argv + 2);
  return 0;
}


#include "../tau/end.hh"


#else

int main()
{
  std::cerr << "γ not defined" << std::endl;
  return 1;
}

#endif
