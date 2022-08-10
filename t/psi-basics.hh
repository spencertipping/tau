#include <unistd.h>


#include "../tau.hh"


#include "../tau/module/begin.hh"

namespace t::psi_basics
{

using namespace std;
using namespace tau;
using namespace tau::flux;
using namespace tau::util;
using tau::operator<<;


void try_io()
{
  cout << "psi begin" << endl;

  Λ l;
  Ψ q(l);

  let a = q.ψc();
  let b = q.ψc();
  let p = q.φc(a, b, 256);

  let f = l.c([&]() {
    i64 t = 0;
    for (iN i = 0; i < 10; ++i)
    {
      let v = u9{i};
      cout << "a → b " << v << endl;
      if (q.ψww(a)) q.ψw(a, v);
      else cout << "ψww == false??" << endl, _exit(1);

      while (q.ψri(a))
      {
        u9 v = q.ψr(a);
        cout << "a got " << v << endl;
      }

      t += i;

      cout << "a→b pipe: " << q.ψwζ(a) << endl;
    }

    q.ψx(a);

    cout << "f is returning " << t << endl;
    return t;
  });

  let g = l.c([&]() {
    i64 t = 0;
    while (q.ψrw(b))
    {
      u9 v = q.ψr(b);
      cout << "b got " << v << endl;
      if (!v.is_greek()) t += Sc<i64>(v);
      if (q.ψww(b)) q.ψw(b, u9{t});
    }

    cout << "final read before closing: " << q.ψr(b) << endl;
    q.ψx(b);

    cout << "g is returning " << t << endl;
    return t;
  });

  cout << "starting Λ..." << endl;
  l.go();

  cout << "final Λ state:" << endl;
  cout << l << endl;

  cout << "ended; λs = " << l.w(f) << ", " << l.w(g) << endl;
  cout << "post-wait Λ state:" << endl;
  cout << l << endl;
}


int main()
{
  try_io();
  return 0;
}


}

#include "../tau/module/end.hh"
