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
      if (r == 0)  { ret = 0; t.close(1); return; }
      if (r == -1) { ret = 1; t.close(1); return; }
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
#else
int cat()
{
  cout << "no cat on this platform" << endl;
  return 1;
}
#endif


int usage(char**);
int main(int argc, char **argv)
{
  τassert_begin;
  if (argc < 2)                return usage(argv);
  if (!strcmp("cat", argv[1])) return cat();
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
