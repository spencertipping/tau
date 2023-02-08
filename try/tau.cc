#include <cstring>
#include <iostream>

#include "../tau.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;


#if τplatform == τplatform_linux
int cat()
{
  τe    t;
  Sp<ψ> q(new ψ{t});
  int   ret = 0;

  q->def([&]() {
    u8 buf[8192];
    while (1)
    {
      let r = t.read(0, buf, sizeof(buf));
      if (r == 0)  { ret = 0; t.close(0); t.close(1); return; }
      if (r == -1) { ret = 1; t.close(0); t.close(1); return; }
      for (iN w = 0; w < r; )
      { let x = t.write(1, buf + w, r - w);
        if (x == -1) { ret = 1; return; }
        w += x; }
    }
    ret = 0;
  });

  t.go();
  return ret;
}


int cat2()
{
  τe t;
  {
    Ξ x{t};
    (γfr(0) | γfw(1))(x);
  }
  A(!γn(), "γs should not exist here; we have " << γn());
  t.go();
  A(!t.l().n(),  "Λ still has λs: "   << t.l().n());
  A(!ξn(),       "ξs still exist: "   << ξn());
  A(!t.pinned(), "t still has pins: " << t.pinned());
  A(!ψn(),       "ψs escaped cat2: "  << ψn());
  return 0;
}


int exec(int argc, char **argv)
{
  τe t;
  {
    Ξ x{t};
    V<St> xs;
    for (int i = 0; i < argc; ++i) xs.push_back(argv[i]);
    auto g = γfr(0) | γfork_exec(xs, "e") | γfw(1);
    g(x);
  }
  A(!γn(), "γs should not exist here; we have " << γn());
  t.go();
  A(!t.l().n(),  "Λ still has λs: "   << t.l().n());
  A(!ξn(),       "ξs still exist: "   << ξn());
  A(!t.pinned(), "t still has pins: " << t.pinned());
  A(!ψn(),       "ψs escaped exec: "  << ψn());
  return 0;
}
#else
int cat()
{
  cout << "no cat on this platform" << endl;
  return 1;
}

int cat2()
{
  cout << "no cat on this platform" << endl;
  return 1;
}

int exec(int, char**)
{
  cout << "no exec on this platform" << endl;
  return 1;
}
#endif


int usage(char**);
int main(int argc, char **argv)
{
  τassert_begin;
  if (argc < 2)                 return usage(argv);
  if (!strcmp("cat",  argv[1])) return cat();
  if (!strcmp("cat2", argv[1])) return cat2();
  if (!strcmp("exec", argv[1])) return exec(argc - 2, argv + 2);
  return 0;
  τassert_end;
}


int usage(char **argv)
{
  cout << "usage: " << argv[0] << " cmd" << endl;
  cout << "where cmd is:" << endl;
  cout << "  cat" << endl;
  cout << endl;
  return 1;
}


#include "../tau/end.hh"
