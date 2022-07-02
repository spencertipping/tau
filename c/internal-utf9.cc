#include <algorithm>
#include <chrono>
#include <iostream>
#include <string_view>

#include "unistd.h"

#include "utf9.hh"


using namespace std;
using namespace tau::utf9;


template <class T> struct debug { T &b; };

template <class T> ostream &operator<<(ostream &s, debug<T> const &d)
{
  for (int i = 0; i < d.b.size();)
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
      0x49, 0x03, 0x01, 0x01, 0x00, 0x80,
      0x40, 0x03, 0x01, 0x01, 0x02, 0x01,

      0x44, 0x14, 0x02,
      0x44, 0x08, 0x04, 0x05,
      // 0x4c, 0x08, 0x05, 0x05, 0x05, 0x05,
        //0x19, 0x00, 0x08,

      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

      0x10,
    };

    ibuf b(buf, sizeof(buf));
    val  x(b,   0);
    val  y(b,   6);
    val  z(b,   12);
    val  a(b,   12 + b.len(12));

    cout << x << " :: " << x.type() << " = " << x.h() << endl;
    cout << y << " :: " << y.type() << " = " << y.h() << endl;
    cout << z << " :: " << z.type() << " = " << z.h() << "; l = " << z.len() << endl;
    cout << a << " :: " << a.type() << " = " << a.h() << endl;
    cout << x << " <=> " << y << " = " << x.compare(y) << endl;

    obuf o1; o1 << z[0];
    ibuf i1(o1.data(), o1.size());
    cout << z[0] << " -> " << debug<obuf>{o1} << "-> " << val(i1, 0) << endl;

    obuf o2; o2 << z[1];
    ibuf i2(o2.data(), o2.size());
    cout << z[1] << " -> " << debug<obuf>{o2} << "-> " << val(i2, 0) << endl;

    obuf o3; z.atype().pack(o3, z[0]); z.atype().pack(o3, z[1]);
    ibuf i3(o3.data(), o3.size());
    cout << val(z.atype(), i3, 0) << ", " << val(z.atype(), i3, 8) << endl;

    for (int64_t upper = 1; upper <= 1048576 * 16; upper *= 16)
    {
      obuf o4;
      val v4 = tau::utf9::tuple();
      for (int64_t i = 0; i < upper; ++i) v4 << val(i);

      {
        auto start = chrono::steady_clock::now();
        o4 << v4;
        auto end   = chrono::steady_clock::now();
        chrono::duration<double> d = end - start;
        cout << upper << " write output: " << d.count() << "s" << endl;
      }

      cout << "buffer size: " << o4.size() << endl;

      ibuf i4(o4.data(), o4.size());

      auto sv = string_view(reinterpret_cast<char const*>(o4.data()),
                            min(o4.size(), 256ul));
      cout << debug<string_view>{sv} << endl;

      cout << val(i4, 0).bsize() << " == " << i4.l << endl;

      {
        auto start = chrono::steady_clock::now();
        auto c     = val(i4, 0).compare(v4);
        auto end   = chrono::steady_clock::now();
        chrono::duration<double> d = end - start;
        cout << upper << ": " << c << ": " << d.count() << "s" << endl;
      }

      {
        auto start = chrono::steady_clock::now();
        uint64_t t = 0; for (auto const &x : val(i4, 0)) t += static_cast<int64_t>(x);
        auto end   = chrono::steady_clock::now();
        chrono::duration<double> d = end - start;
        cout << upper << ": " << t << " == " << (upper * (upper - 1)) / 2 << ": " << d.count() << "s" << endl;
      }

      {
        auto start = chrono::steady_clock::now();
        auto h     = val(i4, 0).h();
        auto end   = chrono::steady_clock::now();
        chrono::duration<double> d = end - start;
        cout << upper << ": " << h << ": " << d.count() << "s" << endl;
      }
    }
  }
  catch (utf9_error const &e)
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
