#include <iostream>
#include "../tau.hh"

#include "../tau/begin.hh"

using namespace std;
using namespace τ;

int main()
{
  u8c       xs[] = { 0x01, 0x02, 0x03, 0x04 };
  Sn<u16bc> ys{Rc<u16bc*>(xs), 2};
  A(ys[0] == 0x0102, "expected 0x0102, got " << ys[0]);
  A(ys[1] == 0x0304, "expected 0x0304, got " << ys[1]);
  cout << "all good" << endl;
  return 0;
}

#include "../tau/end.hh"
