#ifndef τπgc_h
#define τπgc_h


#include "pi-types.hh"

#include "begin.hh"

namespace τ
{


struct πs   // stack allocator
{
  B     a;          // alternating stacks
  B     b;
  bool  c = false;  // alternation flag
  V<uN> d;          // stack registers: offsets into h


  template<class T>
  πs &operator<<(T x) { d.push_back(h << o9(x)); return *this; }

  πs &pop(uN n = 1)
    { A(d.size() >= n, "πs pop(" << n << "), s=" << d.size());
      h.resize(d[d.size() - n + 1]);
      d.resize(d.size() - n);
      return *this; }

  i9 operator[](uN x) { return i9{h.data() + d[d.size() - x]}; }
};


struct πh       // random-access heap (no rewinding)
{
  static constexpr uN  np    = -1;
  static constexpr u8c ni[2] = {Sc<u8>(u9t::none) << 3 | Sc<u8>(u9s::f1), 0};

  B     h;
  V<uN> d;      // data registers: offsets into h
  uN    z = 0;  // available heap space
  u64   a = 0;  // total allocated bytes

  πh(uN n)
    { d.clear();
      d.reserve(n);
      for (uN i = 0; i < n; ++i) d.push_back(np); }


  πh &gc()
    { uN s = 0;
      for (uN i = 0; i < d.size(); ++i)
        if (d[i] != np) s += i9::size_of(h.data() + d[i]);
      B h_;
      h.swap(h_);
      h.reserve(s);  // guarantees no recursive gc
      let a_ = a;
      for (uN i = 0; i < d.size(); ++i)
        if (d[i] != np) set(i, i9{h_.data() + d[i]});
      a = a_;
      return *this; }


  i9 operator[](uN r)
    { return d[r] != np ? i9{h.data() + d[r]} : i9{Cc<u8*>(ni)}; }


  πh &clear(uN r)
    { if (d[r] != np) { z += i9::size_of(h.data() + d[r]); d[r] = np; }
      return *this; }

  template<class T>
  πh &set(uN r, T v)
    { let o = o9(v);
      let s = o.size();
      clear(r);
      if (h.size() + s > h.capacity() && z > h.capacity() >> 2) gc();
      let c = h.size();
      d[r] = h << o;
      a += h.size() - c;
      return *this; }
};


#if τdebug_iostream
O &operator<<(O &s, πs const &h)
{
  auto &hm = const_cast<πs&>(h);
  s << "πs n=" << h.d.size() << std::endl;
  for (uN i = 0; i < h.d.size(); ++i)
    s << "  " << i << "\t" << hm[i] << std::endl;
  return s;
}

O &operator<<(O &s, πh const &h)
{
  auto &hm = const_cast<πh&>(h);
  s << "πh c=" << h.h.capacity() << " wa=" << h.h.capacity() - h.h.size() << ", a=" << h.a << std::endl;
  for (uN i = 0; i < h.d.size(); ++i)
    s << "  r" << i << "\t" << hm[i] << std::endl;
  return s;
}
#endif


}

#include "end.hh"


#endif
