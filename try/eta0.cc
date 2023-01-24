#include <iostream>
#include "../tau.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;


void test_uint(u64 i)
{
  u8 buf[64] = {0};
  η0o o = i; o.into(buf);
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


void test_int(i64 i)
{
  u8 buf[64] = {0};
  η0o o = i; o.into(buf);
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


void try_ints()
{
  for (uN i = 0; i < 1048576; ++i)
    for (uN j = 0; j < 64; j += 9)
      test_uint(i << j);

  for (iN i = -1048576; i < 1048576; ++i)
    for (uN j = 0; j < 64; j += 9)
      test_int(i << j);
}


void try_strings()
{
  u8 buf[1024] = {0};
  η0o o;
  o.t(η0t::bytes).c(19).h(true) << "fu" << "bar";
  cout << "output size = " << o.osize() << endl;

  o.into(buf);

  for (uN j = 0; j < o.osize(); ++j)
    cout << Sc<int>(buf[j]) << " ";
  cout << endl;

  η0i i = buf;
  cout << i << " = " << i.stv(64) << ", v = " << i.v() << endl;

  cout << "calc = " << picosha3::bytes_to_hex_string(i.sha3()) << endl;
  cout << "stor = " << picosha3::bytes_to_hex_string(i.stored_sha3()) << endl;
}


int main()
{
  τassert_begin;
  //try_ints();
  try_strings();
  return 0;
  τassert_end;
}


#include "../tau/end.hh"
