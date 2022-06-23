#include <iostream>

#include "unistd.h"

#include "utf9.hh"


using namespace std;
using namespace tau::utf9;


#define p reinterpret_cast<void*>
#define n reinterpret_cast<uint64_t>


void try_loading_stuff()
{
  try
  {
    uint8_t buf[] = {0x01, 0x03, 0x00, 0x01, 0x02, 0x01};
    ibuf b(buf, sizeof(buf));
    val  x(b,   0);
    val  y(b,   3);

    cout << "xlen = " << b.len(0) << endl;
    cout << x.type() << endl;
    cout << static_cast<uint64_t>(x) << endl;
    cout << x.h() << endl;
    cout << x << endl;

    cout << y.type() << endl;
    cout << static_cast<uint64_t>(y) << endl;
    cout << y.h() << endl;
    cout << y << endl;

    cout << x.compare(y) << endl;
  }
  catch (error e)
  {
    cout << "ERROR " << e << endl;
    _exit(1);
  }
}


int main()
{
  try_loading_stuff();

  void *min = p(lfns[0]);
  void *max = p(lfns[0]);

  for (int i = 0; i < 255; ++i)
  {
    if (p(lfns[i]) < min) min = p(lfns[i]);
    if (p(lfns[i]) > max) max = p(lfns[i]);

    if (p(tlfns[i]) < min) min = p(tlfns[i]);
    if (p(tlfns[i]) > max) max = p(tlfns[i]);

    if (p(tvlfns[i]) < min) min = p(tvlfns[i]);
    if (p(tvlfns[i]) > max) max = p(tvlfns[i]);
  }

  cout << "min = " << min << endl;
  cout << "max = " << max << endl;

  cout << "max - min = " << (n(max) - n(min)) << endl;
  return 0;
}
