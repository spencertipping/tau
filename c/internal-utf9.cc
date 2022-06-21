#include <iostream>

#include "utf9.hh"


using namespace std;
using namespace tau::utf9;


#define p reinterpret_cast<void*>
#define n reinterpret_cast<uint64_t>

int main()
{
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
