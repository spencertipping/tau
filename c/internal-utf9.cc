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
    uint8_t buf[] = {
      0x49, 0x03, 0x01, 0x03, 0x00, 0x80,
      0x40, 0x03, 0x01, 0x01, 0x02, 0x01,

      0x44, 0x10, 0x04, 0x4a, 0x04, 0x01, 0x01,
        0x00, 0x01, 0x01, 0x00,
        0x00, 0x02, 0x02, 0x00,
        0x00, 0x03, 0x04, 0x00,
        0x00, 0x04, 0x08, 0x00,

      0x0a,
    };

    ibuf b(buf, sizeof(buf));
    val  x(b,   0);
    val  y(b,   6);
    val  z(b,   12);
    val  a(b,   12 + b.len(12));

    cout << "xlen = " << b.len(0) << endl;
    cout << x << " :: " << x.type() << endl;
    cout << x.h() << endl;

    cout << y << " :: " << y.type() << endl;
    cout << y.h() << endl;

    cout << z << endl;
    cout << z.h() << endl;

    cout << a << " :: " << a.type() << endl;

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
