#include <iostream>
#include "../tau.hh"
#include "../tau/begin.hh"

#include "../dep/picosha3.h"


using namespace τ;
using namespace std;


void test_uint(u64 i)
{
  u8 buf[64] = {0};
  η0o o = η1o(i); o.into(buf);
  A(η0bc(buf, sizeof(buf)), "η₀bc failed on " << i);
  η0i r{buf};
  A(r.type() == η0t::uint_be,
    r.type() << " ≠ " << η0t::uint_be << "; r = " << r);
  let d = r.data();
  u64 x = 0;
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
  η0o o = η1o(i); o.into(buf);
  A(η0bc(buf, sizeof(buf)), "η₀bc failed on " << i);
  η0i r{buf};
  A(r.type() == η0t::int_be,
    r.type() << " ≠ " << η0t::uint_be << "; r = " << r);
  let d = r.data();
  i64 x = *d & 128 ? -1 : 0;
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
  for (u64 i = 0; i < 1024; ++i)
    for (u64 j = 0; j < 64; j += 9)
      test_uint(i << j);

  for (i64 i = -1024; i < 1024; ++i)
    for (u64 j = 0; j < 64; j += 9)
      test_int(i << j);
}


void try_strings()
{
  u8 buf[1024] = {0};
  η0o o(η0t::bytes);
  o.h(true) << "fu" << "bar";
  if (τhas_zstd) o.c(19);
  cout << "output size = " << o.osize() << endl;

  o.into(buf);
  for (uN j = 0; j < o.osize(); ++j)
    cout << Sc<int>(buf[j]) << " ";

  A(η0bc(buf, sizeof(buf)), "!η0bc()");
  cout << endl;
  cout << "about to get stringview" << endl;

  η0i i = buf;
  cout << i << " = " << i.stv(64) << endl;
  cout << "v = " << i.v() << endl;

  cout << "calc = " << picosha3::bytes_to_hex_string(i.sha3()) << endl;
  cout << "stor = " << picosha3::bytes_to_hex_string(i.stored_sha3()) << endl;

  A(i.v(), "η failed to validate hashed value");
}


void try_small_tuples()
{
  u8 buf[1024] = {0};
  η0o o(η0t::tuple);
  o << η1o("fu") << η1o("bar") << η1o(3) << η1o(4.0) << η1o(false);
  cout << "output size = " << o.osize() << endl;

  o.into(buf);

  A(η0bc(buf, o.osize()), "failed bounds check");
  cout << "BUF = " << Sc<void*>(buf) << endl;

  let y0 = η0i{buf};
  let y1 = η{buf};

  auto it = y1.T().begin();
  cout << *it << endl; A((*it).st() == St{"fu"}, "expected fu, got " << (*it).ps()); ++it;
  cout << *it << endl; A((*it).st() == St{"bar"}, "expected bar, got " << (*it).ps()); ++it;
  cout << *it << endl; A((*it).pi() == 3, "expected 3, got " << (*it).pi()); ++it;
  cout << *it << endl; A((*it).pf() == 4.0, "expected 4, got " << (*it).pf()); ++it;
  cout << *it << endl; A((*it).pb() == false, "expected false, got " << (*it).pb()); ++it;
  A(it == y1.T().end(),
    "mismatching addresses: "
    << Sc<void const*>(it.x) << " vs "
    << Sc<void const*>(y1.T().end().x));

  cout << y0 << endl;
  cout << y1 << endl;
  A(y1.len() == 5, "bogus len for y1: " << y1.len());
}


void try_large_tuples()
{
  B buf; buf.resize(1048576);
  let d = buf.data();
  let c = buf.capacity();

  uN tests = 0;

  for (u64 i = 0; i < 80000; i += i % 1997 + 1)
  {
    cout << "i = " << i << endl;
    ++tests;
    η0o o(η0t::tuple);
    o.h(i % 3 == 0);
    if (τhas_zstd) o.c(i % 20);
    for (u64 j = 0; j <= i; ++j) o << η1o(j);
    o.into(d);

    A(η0bc(d, c), "η0bc failed for i = " << i);
    η r{d};

    uN t = 0;
    uN inner = 0;
    for (let &x : r.T())
    {
      ++inner;
      A(x.tu(), "!x.tu, i = " << i << ", t = " << t << ", x = "
        << x << ", inner = " << inner << ", r = " << r
        << ", r η0 = " << r.y0());
      t += x.pu();
    }
    A(t == (i * (i + 1)) / 2,
      "summation mismatch for i = " << i << ": " << t
      << " ≠ " << (i * (i + 1)) / 2);
  }

  cout << tests << " large tuples all good" << endl;
}


void tuple_bench()
{
  B buf; buf.reserve(1048576 * 64);
  let d = buf.data();

  cout << "starting tuple_bench write loop" << endl;
  let t0 = now();
  for (u64 i = 0; i < 10; ++i)
  {
    η0o o(η0t::tuple);
    for (u64 j = 0; j < 1048576; ++j) o << η1o(j);
    o.into(d);
  }
  let t1 = now();

  cout << "writing 1048576 ints into a tuple: "
       << (t1 - t0) / 10 << endl;

  let t2 = now();
  u64 t = 0;
  for (u64 i = 0; i < 10; ++i)
  {
    η r{d};
    for (let &x : r.T()) t += x.pu();
  }
  let t3 = now();

  cout << "reading 1048576 ints from a tuple: "
       << (t3 - t2) / 10
       << " (total is " << t << ")"
       << endl;
}


int main()
{
  τassert_begin;
  try_ints();
  try_strings();
  try_small_tuples();
  try_large_tuples();
  tuple_bench();
  return 0;
  τassert_end;
}


#include "../tau/end.hh"
