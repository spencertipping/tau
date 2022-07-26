#include <algorithm>
#include <chrono>
#include <iostream>
#include <string_view>

#include "unistd.h"


#include "../tau.hh"

#include "../tau/module/begin.hh"


using namespace std;
using namespace tau;


template <class T> struct debug { T &b; };

template <class T> ostream &operator<<(ostream &s, debug<T> const &d)
{
  s << "len=" << d.b.size() << ":" << endl;
  for (unsigned i = 0; i < d.b.size();)
  {
    uint8_t c = d.b.data()[i];
    s.put("0123456789abcdef"[c >> 4]);
    s.put("0123456789abcdef"[c & 15]);
    s.put(++i & 15 ? ' ' : '\n');
  }
  return s;
}


void try_loading_stuff()
{
  try
  {
    uint8_t buf[] = {
      0x49, 0x03, 0x05, 0xff, 0xf0, 0x80,
      0x40, 0x03, 0x01, 0x05, 0x02, 0x01,

      0x44, 0x14, 0x02,
      0x44, 0x08, 0x04, 0x05,
      // 0x4c, 0x08, 0x05, 0x05, 0x05, 0x05,
        //0x19, 0x00, 0x08,

      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

      0x10,
    };

    i9 b(buf, sizeof(buf));
    u9  x(b,   0);
    u9  y(b,   6);
    u9  z(b,   12);
    u9  a(b,   12 + b.len(12));

    cout << x << " :: " << x.type() << " = " << x.h() << endl;
    cout << y << " :: " << y.type() << " = " << y.h() << endl;
    cout << z << " :: " << z.type() << " = " << z.h() << "; l = " << z.len() << endl;
    cout << a << " :: " << a.type() << " = " << a.h() << endl;
    cout << x << " <=> " << y << " = " << x.compare(y) << endl;

    o9 o1; o1 << z[0];
    i9 i1(o1.data(), o1.size());
    cout << z[0] << " -> " << debug<o9>{o1} << "-> " << u9(i1, 0) << endl;

    o9 o2; o2 << z[1];
    i9 i2(o2.data(), o2.size());
    cout << z[1] << " -> " << debug<o9>{o2} << "-> " << u9(i2, 0) << endl;

    o9 o3; z.atype().pack(o3, z[0]); z.atype().pack(o3, z[1]);
    i9 i3(o3.data(), o3.size());
    cout << u9(z.atype(), i3, 0) << ", " << u9(z.atype(), i3, 8) << endl;

    for (int64_t upper = 1; upper <= 1048576 * 16; upper *= 16)
    {
      o9 o4;
      u9 v4 = tau::utf9::tuple(upper);
      for (int64_t i = 0; i < upper; ++i) v4 << u9(i);

      {
        auto start = chrono::steady_clock::now();
        o4 << v4;
        auto end   = chrono::steady_clock::now();
        chrono::duration<double> d = end - start;
        cout << upper << " write output: " << d.count() << "s" << endl;
      }

      cout << "buffer size: " << o4.size() << endl;

      i9 i4(o4.data(), o4.size());

      auto sv = string_view(reinterpret_cast<char const*>(o4.data()),
                            min(static_cast<unsigned long long>(o4.size()), 256ull));
      //cout << debug<string_view>{sv} << endl;

      cout << u9(i4, 0).bsize() << " == " << i4.l << endl;

      {
        auto start = chrono::steady_clock::now();
        auto c     = u9(i4, 0).compare(v4);
        auto end   = chrono::steady_clock::now();
        chrono::duration<double> d = end - start;
        cout << upper << ": " << c << ": " << d.count() << "s" << endl;
      }

      {
        std::vector<int64_t> xs;
        xs.reserve(upper);
        for (int64_t i = 0; i < upper; ++i) xs.push_back(i);
        auto start = chrono::steady_clock::now();
        uint64_t t = 0; for (auto const &x : xs) t += x;
        auto end   = chrono::steady_clock::now();
        chrono::duration<double> d = end - start;
        cout << "native sum: " << t << ": " << d.count() << "s" << endl;
      }

      {
        auto start = chrono::steady_clock::now();
        uint64_t t = 0; for (auto const &x : v4) t += static_cast<int64_t>(x);
        auto end   = chrono::steady_clock::now();
        chrono::duration<double> d = end - start;
        cout << "isum: " << t << ": " << d.count() << "s" << endl;
      }

      {
        auto start = chrono::steady_clock::now();
        uint64_t t = 0; for (auto const &x : u9(i4, 0)) t += static_cast<int64_t>(x);
        auto end   = chrono::steady_clock::now();
        chrono::duration<double> d = end - start;
        cout << upper << ": " << t << " == " << (upper * (upper - 1)) / 2 << ": " << d.count() << "s" << endl;
      }

      {
        auto start = chrono::steady_clock::now();
        auto h     = u9(i4, 0).h();
        auto end   = chrono::steady_clock::now();
        chrono::duration<double> d = end - start;
        cout << upper << ": " << h << ": " << d.count() << "s" << endl;
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
    auto v = tau::utf9::tuple();
    for (int64_t i = 0; i < 100; ++i) v << u9(i);
    for (int64_t i = -10; i < 110; ++i)
      if (v.to<u9::kf_te>(u9(i), none).exists() != (i >= 0 && i < 100))
        cout << "ord find mismatch for " << i << endl;

    auto vh = v.make_th<u9::kf_te>();
    for (int64_t i = -10; i < 110; ++i)
      if (vh.th<u9::kf_te>(u9(i), none).exists() != (i >= 0 && i < 100))
        cout << "hash find mismatch for " << i << endl;
  }
  catch (u9e const &e)
  {
    cout << "ERROR " << e << endl;
    _exit(1);
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


int main()
{
  cout << "native endianness: "
       << (std::endian::native == std::endian::big ? "big" : "little") << endl;

  cout << "try_orderings"      << endl; try_orderings();
  cout << "try_printing_types" << endl; try_printing_types();
  cout << "try_loading_stuff"  << endl; try_loading_stuff();
  return 0;
}


#include "../tau/module/end.hh"
