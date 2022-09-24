#ifndef τπgc_h
#define τπgc_h


#include "pi-types.hh"

#include "begin.hh"

namespace τ
{


struct πs   // stack allocator
{
  sc uN np = -1;

  B     a;  // alternating stacks; a's tag is 0
  B     b;  //              ...and b's tag is 1
  bool  c = false;
  V<uN> d;  // stack registers: tagged offsets


  uN last(bool tag) const
    { for (auto it = d.rbegin(); it != d.rend(); ++it)
        if ((*it & 1) == tag)
          return *it >> 1;
      return np; }

  uN size () const { return a.size() + b.size(); }
  uN depth() const { return d.size(); }

  B &h() { return c ? b : a; }
  B &o() { return c ? a : b; }


  template<class T>
  πs &operator<<(T x)
    { c = !c;
      d.push_back(h() << o9(x) << 1 | c);
      let l = last(!c);
      if (l != np) o().resize(l + i9::size_of(o().data() + l));
      return *this; }

  πs &pop(uN n = 1)
    { A(d.size() >= n, "πs pop(" << n << "), s=" << d.size());
      d.resize(d.size() - n);
      return *this; }

  i9 operator[](uN x)
    { let n = d[d.size() - x - 1];
      return i9{(n & 1 ? b : a).data() + (n >> 1)}; }
};


struct πh       // random-access heap (no rewinding)
{
  sc uN  np    = -1;
  sc u8c ni[2] = {Sc<u8>(u9t::none) << 3 | Sc<u8>(u9s::v8), 0};

  B     h;
  V<uN> d;      // data registers: offsets into h
  uN    z = 0;  // available heap space
  uN    g = 0;  // generations
  u64   a = 0;  // total allocated bytes

  πh(uN n)
    { d.clear();
      d.reserve(n);
      for (uN i = 0; i < n; ++i) d.push_back(np); }


  uN size() const { return h.size(); }

  πh &gc()
    { uN s = 0;
      for (uN i = 0; i < d.size(); ++i)
        if (d[i] != np) s += i9::size_of(h.data() + d[i]);
      B h_;
      h.swap(h_);
      h.reserve(s * 2);
      let a_ = a;
      for (uN i = 0; i < d.size(); ++i)
        if (d[i] != np) set(i, i9{h_.data() + d[i]});
      a = a_;
      ++g;
      z = 0;
      return *this; }


  i9 operator[](uN r)
    { return d[r] != np ? i9{h.data() + d[r]} : i9{Cc<u8*>(ni)}; }


  πh &clear(uN r)
    { if (d[r] != np) { z += i9::size_of(h.data() + d[r]); d[r] = np; }
      return *this; }

  template<class T>
  πh &set(uN r, T const &v)
    { let o = o9(v);
      let s = o.size();
      clear(r);
      if (h.size() + s > h.capacity() && z > h.capacity() >> 2) gc();
      let c = h.size();
      d[r] = h << o;
      a += h.size() - c;
      return *this; }

  // NOTE: below methods are unsafe for mixed push/pop scenarios
  // (for safety, use πs)

  template<class T>
  uN operator<<(T const &v)
    { let r = d.size();
      d.push_back(np);
      set(r, v);
      return r; }

  πh &pop(uN n = 1)
    { d.resize(d.size() - n);
      return *this; }
};


#if τdebug_iostream
O &operator<<(O &s, πs const &h)
{
  auto &hm = const_cast<πs&>(h);
  s << "πs s=" << h.size() << " n=" << h.d.size() << std::endl;
  for (uN i = 0; i < h.d.size(); ++i)
    s << "  s" << i << "\t" << h.d[h.d.size() - i - 1] << "\t" << hm[i] << std::endl;
  return s;
}

O &operator<<(O &s, πh const &h)
{
  auto &hm = const_cast<πh&>(h);
  s << "πh c=" << h.h.capacity()
    << " wa="  << h.h.capacity() - h.h.size()
    << " a="   << h.a
    << " z="   << h.z
    << " g="   << h.g << std::endl;
  for (uN i = 0; i < h.d.size(); ++i)
    s << "  r" << i << "\t" << hm[i] << std::endl;
  return s;
}
#endif


}

#include "end.hh"


#endif
