#include <iostream>
#include "../tau.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;

void try_ints()
{
  u8 buf[64] = {0};
  for (uN i = 0; i < 1048576; ++i)
  {
    η0o o = i; o.into(buf, sizeof(buf));
    A(η0bc(buf, sizeof(buf)), "η₀bc failed on " << i);
    η0i r{buf};
    A(r.type() == η0t::uint_be,
      r.type() << " ≠ " << η0t::uint_be << "; r = " << r);
    let d = r.data();
    uN  x = 0;
    for (uN j = 0; j < r.size(); ++j) x = x << 8 | d[j];
    A(x == i,
      "uint_be decode fail: " << x << " ≠ " << i << "; r = " << r
      << "; buf = " << Sc<uN>(buf[0])
             << " " << Sc<uN>(buf[1])
             << " " << Sc<uN>(buf[2])
             << " " << Sc<uN>(buf[3])
             << " " << Sc<uN>(buf[4])
             << " " << Sc<uN>(buf[5])
             << " " << Sc<uN>(buf[6]));
  }

  for (iN i = -1048576; i < 1048576; ++i)
  {
    η0o o = i; o.into(buf, sizeof(buf));
    A(η0bc(buf, sizeof(buf)), "η₀bc failed on " << i);
    η0i r{buf};
    A(r.type() == η0t::int_be,
      r.type() << " ≠ " << η0t::uint_be << "; r = " << r);
    let d = r.data();
    iN  x = *d & 128 ? -1 : 0;
    for (uN j = 0; j < r.size(); ++j) x = x << 8 | d[j];
    A(x == i,
      "int_be decode fail: " << x << " ≠ " << i << "; r = " << r
      << "; buf = " << Sc<uN>(buf[0])
             << " " << Sc<uN>(buf[1])
             << " " << Sc<uN>(buf[2])
             << " " << Sc<uN>(buf[3])
             << " " << Sc<uN>(buf[4])
             << " " << Sc<uN>(buf[5])
             << " " << Sc<uN>(buf[6]));
  }
}


int main()
{
  τassert_begin;
  try_ints();
  return 0;
  τassert_end;
}


#include "../tau/end.hh"
