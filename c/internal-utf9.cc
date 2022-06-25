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
  utf9_init();
  try_loading_stuff();
  return 0;
}
