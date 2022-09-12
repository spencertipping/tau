//#define tau_debug_i9st 1

#include "../tau.hh"

#if τdefines_γ


#include <cstring>


#include "../tau/begin.hh"

using namespace std;
using namespace τ;
using namespace τ::ξ;


int nop_seq(int argc, char **argv)
{
  Φ f;
  for (let x : ι(atoi(argv[0]))) cout << f.dt() << ": " << x << endl;
  return 0;
}


int tau_seq(int argc, char **argv)
{
  Φ f;
  iota(f, atoi(argv[0])) | Δθ(f, 50ms) | stream_out(f, std::cout);
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


int x11(int argc, char **argv)
{
  Φ f;
  let t0 = now();

  auto &s = xframe(f) | splitter(f);
  auto &r = θr(f, 30ms) | each<i9>(f, [t0](i9, γφ &p) {
    let x = 100.f + (now() - t0) / 10ms % 500;
    let y = 50.f;

    for (uN dy = 1; dy <= 16; dy += 5)
      p << o9t((uN) xrop_line, x,
               y - dy, x + 500 - 10*dy, y - dy, 1.f, 0xe0e0f0ff);

    p << o9t((uN) xrop_text, "Gentium 48",
             "τ« //ξ", 0xe0e0f0ff, x, 50.f);

    p << u9τ;
  });

  r ^ s;
  s & stream_out(f, cout);

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
  if (!strcmp(argv[1], "x11"))  return x11     (argc - 2, argv + 2);
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
