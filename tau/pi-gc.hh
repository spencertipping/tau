#ifndef τπgc_h
#define τπgc_h


#include "pi-types.hh"

#include "begin.hh"

namespace τ
{


struct πs       // stack allocator
{
  B     h;      // linear memory
  V<iN> d;      // stack registers: offsets into h
  u64   a = 0;  // total allocated bytes


  template<class T>
  iN operator<<(T x)
    { let o = o9(x);
      let s = o.size();
      let c = h.size();
      h.resize(c + s);
      if (let n = o.write(h.data() + c))
      { A(n != ζω, "πs allocation failed");
        h.resize(h.size() - s + n); }
      a += h.size() - c;
      d.push_back(c);
      return c; }

  πs &pop(uN n = 1)
    { h.resize(d[d.size() - n + 1]);
      d.resize(d.size() - n);
      return *this; }

  i9 operator[](iN x)
    { return i9{h.data() + (x < 0 ? d[d.size() + x] : d[x])}; }
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
      h.resize(c + s);
      if (let n = o.write(h.data() + c))
      { A(n != ζω, "πh allocation failed");
        h.resize(h.size() - s + n); }
      d[r] = c;
      return *this; }
};


#if τdebug_iostream
O &operator<<(O &s, πs const &h)
{
  auto &hm = const_cast<πs&>(h);
  s << "πs n=" << h.d.size() << ", a=" << h.a << std::endl;
  for (uN i = 1; i <= h.d.size(); ++i)
    s << "  " << i - 1 << "\t" << hm[-i] << std::endl;
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
