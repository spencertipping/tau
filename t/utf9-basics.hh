#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <string_view>

#include "unistd.h"


#ifndef QUICK
#define QUICK 0
#endif


#include "../tau.hh"


#include "../tau/module/begin.hh"

namespace t::utf9_basics
{

using namespace std;
using namespace tau;
using namespace tau::util::numerics;
using namespace tau::util;
using tau::operator<<;


template <class T> struct debug { T &b; };

template <class T> ostream &operator<<(ostream &s, debug<T> const &d)
{
  s << "len=" << d.b.size() << ":" << endl;
  for (unsigned i = 0; i < d.b.size();)
  {
    u8 c = d.b.data()[i];
    s.put("0123456789abcdef"[c >> 4]);
    s.put("0123456789abcdef"[c & 15]);
    s.put(++i & 15 ? ' ' : '\n');
  }
  return s;
}


void try_really_simple()
{
  try
  {
    u8c buf[] = {
      0x00, 0x7f,
      0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
      0x4b, 0x03, 0x81, 0x82, 0x83,
    };
    i9 b(buf, sizeof(buf));
    u9 x(b,   0);
    u9 y(b,   x.i + b.len(x.i));
    u9 z(b,   y.i + b.len(y.i));

    cout << "uint8 127 = "    << x.i << ">" << b.len(x.i) << ": " << x << endl;
    cout << "uint64 65536 = " << y.i << ">" << b.len(y.i) << ": " << y << endl;
    cout << "fixtuple3 = "    << z.i << ">" << b.len(z.i) << ": " << z << endl;
  }
  catch (u9e const &e)
  {
    cout << "ERROR " << e << endl;
    _exit(1);
  }
}


void try_loading_stuff()
{
  u8c buf[] = {
    0x49, 0x03, 0x05, 0xff, 0xf0, 0x80,
    0x40, 0x03, 0x01, 0x05, 0x02, 0x01,

    0x44, 0x14, 0x02,          // array8 len=20 n=2
    0x44, 0x08, 0x04, 0x05,    //   t = array8 len=8 n=4 t=uint16
    0x00, 0x01, 0x02, 0x03,  // [0][0]
    0x04, 0x05, 0x06, 0x07,  // [0][1]
    0x08, 0x09, 0x0a, 0x0b,  // [1][0]
    0x0c, 0x0d, 0x0e, 0x0f,  // [1][1]

    0x10,
  };

  i9 b(buf, sizeof(buf));
  u9 x(b,   0);
  u9 y(b,   6);
  u9 z(b,   12);
  u9 a(b,   12 + b.len(12));

  o9 o1;
  o9 o2;
  o9 o3;

  try
  {
    assert(12 + b.len(12) < b.size());

    cout << x << " :: " << x.type() << " = " << x.h() << endl;
    cout << y << " :: " << y.type() << " = " << y.h() << endl;
    cout << z
         << " :: " << z.type()
         << " = " << z.h()
         << "; l = " << z.len() << endl;
    cout << a << " :: " << a.type() << " = " << a.h() << endl;
    cout << x << " <=> " << y << " = " << x.compare(y) << endl;

    o1 << z[0];
    i9 i1(o1.data(), o1.size());
    cout << z[0] << endl;
    cout << " -> " << debug<i9>{i1} << endl;
    cout << " -> " << u9(i1, 0) << endl;

    o2 << z[1];
    i9 i2(o2.data(), o2.size());
    cout << z[1] << " -> " << debug<o9>{o2} << "-> " << u9(i2, 0) << endl;

    z.atype().pack(o3, z[0]); z.atype().pack(o3, z[1]);
    i9 i3(o3.data(), o3.size());
    cout << u9(z.atype(), i3, 0) << ", " << u9(z.atype(), i3, 8) << endl;
  }
  catch (u9e const &e)
  {
    cout << "ERROR " << e << endl;
    _exit(1);
  }
}


void try_bench()
{
  o9 o4;
  try
  {
    for (iN upper = 1; upper <= (QUICK ? 16384 : 1048576 * 16); upper *= 16)
    {
      u9 v4 = u9t(upper);
      for (i64 i = 0; i < upper; ++i) v4 << u9(i, tau::utf9::INT);

      {
        o4.clear();
        let start = now();
        o4 << v4;
        let end = now();
        cout << upper << " write output: " << end - start << endl;
      }

      cout << "buffer size: " << o4.size() << endl;

      i9 i4(o4.data(), o4.size());

      //auto sv = string_view(reinterpret_cast<char const*>(o4.data()),
      //                      min(static_cast<unsigned long long>(o4.size()), 256ull));
      //cout << debug<string_view>{sv} << endl;

      cout << u9(i4, 0).bsize() << " == " << i4.l << endl;

      {
        let start = now();
        let c     = u9(i4, 0).compare(v4);
        let end   = now();
        cout << upper << ": " << c << ": " << end - start << endl;
      }

      {
        V<i64> xs;
        xs.reserve(upper);
        for (iN i = 0; i < upper; ++i) xs.push_back(i);
        let start = now();
        u64 t = 0; for (auto const &x : xs) t += x;
        let end   = now();
        cout << "csum: " << t << ": " << end - start << endl;
      }

      {
        let start = now();
        i64 t = 0; for (auto const &x : v4) t += static_cast<i64>(x);
        let end   = now();
        cout << "isum: " << t << ": " << end - start << endl;
      }

      {
        let start = now();
        i64 t = 0; for (auto const &x : u9(i4, 0)) ++t;
        let end   = now();
        cout << "decode: " << end - start << endl;
      }

      {
        let start = now();
        i64 t = 0; for (auto const &x : u9(i4, 0)) t += static_cast<i64>(x);
        let end   = now();
        cout << "decode+sum: " << t << " == " << (upper * (upper - 1)) / 2 << ": " << end - start << endl;
      }

      {
        let start = now();
        let h     = u9(i4, 0).h();
        let end   = now();
        cout << "hash " << upper << ": " << h << ": " << end - start << endl;
      }
    }
  }
  catch (u9e const &e)
  {
    cout << "ERROR " << e << endl;
    _exit(1);
  }
}


void try_orderings()
{
  try
  {
    auto v = u9t();
    for (i64 i = 0; i < 100; ++i) v << u9(i, tau::utf9::INT);

    auto vh = v.make_th<u9::kf_te>();
    if (!vh.is_th<u9::kf_te>())
    { cout << "not hash-ordered after make_th" << endl;
      cout << "INITIAL ORDERING" << endl;
      for (let &x : v) cout << "  " << x << ": " << x.h() << endl;
      cout << endl;
      cout << "SORTED ORDERING" << endl;
      for (let &x : vh) cout << "  " << x << ": " << x.h() << endl;
      _exit(1); }

    for (i64 i = -10; i < 110; ++i)
      if (vh.th<u9::kf_te>(u9(i, tau::utf9::INT), u9n).exists() != (i >= 0 && i < 100))
      { cout << "hash find mismatch for " << i << endl;
        _exit(1); }
  }
  catch (u9e const &e)
  {
    cout << "ERROR " << e << endl;
    _exit(1);
  }
}


void try_coercion_error()
{
  try
  {
    u8c buf[] = {
      0x00, 0x7f,
    };
    i9 b(buf, sizeof(buf));
    u9 x(b,   0);

    cout << static_cast<i64>(x) << endl;
    cout << "the previous cast should have failed" << endl;
    _exit(1);
  }
  catch (u9e const &e)
  {
    cout << "correctly caught error: " << e << endl;
  }
}


void try_printing_types()
{
  try
  {
    cout << t9(tu8) << endl;
    cout << t9(ttuple({tu8, tu16})) << endl;
    cout << t9(ttuple({tu8, tu16, tu32, tu64, ti8, ti16, ti32, ti64})) << endl;
    cout << t9(ttuple({tu8, ttuple({tu16, tu32, tu64}), ti8, ti16, ti32, ti64})) << endl;
    cout << t9(ttuple({tarray(100ul, ttuple({tu64, tu32})), tu64})) << endl;
  }
  catch (u9de const &e)
  {
    cout << "ERROR " << e << endl;
    cout << debug<i9 const>{e.b} << endl;
    _exit(1);
  }
  catch (u9e const &e)
  {
    cout << "ERROR " << e << endl;
    _exit(1);
  }
}


void try_bit_preferences()
{
  o9 b;
  for (uN i = 1; i < 10000000; i *= 7)
    for (uN lb = 0; lb <= 4; ++lb)
    {
      let n = i;
      let l = i << lb;
      cout << "n = " << i << "; l = " << l << ": ";

      let lbits = (int) b.list_pos_bits(n, l);
      let sbits = (int) b.set_rcp_bits(n, l);
      cout << "list_pos_bits = " << lbits << " = " << (n     >> lbits) << " x " << (int) su(l) << "; ";
      cout << "set_rcp_bits  = " << sbits << " = " << (~0ull >> sbits) << " x " << (int) su(l) << endl;

      if (lbits < 64 && (n >> lbits) * su(l) > l >> b.INDEX_SIZE_BITS)
      {
        cout << "ERROR: oversize list_pos index" << endl;
        _exit(1);
      }

      if (sbits < 64 && (~0ull >> sbits) * su(l) > l >> b.INDEX_SIZE_BITS)
      {
        cout << "ERROR: oversize set_rcp index" << endl;
        _exit(1);
      }
    }
}


void try_symbols()
{
  u9 a = u9y("foo");
  u9 b = u9y("foo");
  u9 c = u9y("bar");

  assert(a == b);
  assert(c > b);

  cout << "symbols:" << endl;
  cout << "  foo = " << a << endl;
  cout << "  foo = " << b << endl;
  cout << "  bar = " << c << endl;
}


int main()
{
  cout << "native endianness: "
       << (std::endian::native == std::endian::big ? "big" : "little") << endl;

  cout << "try_symbols"         << endl; try_symbols();
  cout << "try_bit_preferences" << endl; try_bit_preferences();
  cout << "try_really_simple"   << endl; try_really_simple();
  cout << "try_coercion_error"  << endl; try_coercion_error();
  cout << "try_loading_stuff"   << endl; try_loading_stuff();
  cout << "try_orderings"       << endl; try_orderings();
  cout << "try_bench"           << endl; try_bench();
  cout << "try_printing_types"  << endl; try_printing_types();
  return 0;
}


}

#include "../tau/module/end.hh"
