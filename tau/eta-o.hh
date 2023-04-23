#ifndef τηo_h
#define τηo_h

#include "Lambda.hh"
#include "xi.hh"
#include "eta-types.hh"
#include "eta-i.hh"

#include "begin.hh"

namespace τ
{


// ηoc: output container for ηo writers
template<class T> struct ηoc;

template<> struct ηoc<ξ>
{
  bool   expired()    { return o.expired(); }
  Sn<u8> iptr(uN n)   { return wpg(o)->iptr(n); }
  void   commit(uN n) { wpg(o)->commit(n); }
  void   abort()      { wpg(o)->abort(); }

  Wp<ξ> o;
};

template<> struct ηoc<B>
{
  bool   expired()    { return false; }
  Sn<u8> iptr(uN n)   { b.resize(n); return {b.data(), b.size()}; }
  void   commit(uN n) {}
  void   abort()      { b.clear(); }

  B b;
};


// Inline output writer for η values, in this case into a ξ.
//
// This object accepts a stream of values that may overflow the initial
// allocation. If this happens, the next stream operation may block, but your
// memory will be safe; that is, aside from blocking you don't need to care
// about the details of the underlying allocation.
//
// NOTE: we cannot hold a strong reference to the underlying ξ, so we must
// tolerate it being deallocated while this object exists. If that happens,
// writer methods will silently become no-ops. If you create a ηo against an
// expired pointer, it will SIGPIPE and terminate the calling λ.

template<class T>
struct ηo final
{
  ηo(ηoc<T> o, uN c0 = 256) : o_(o), s_(0)
    { A(c0, "ηo with no initial capacity");
      if (!o_.expired()) b_ = o_.iptr(c0);
      else               λx(), τunreachable(); }

  ~ηo()
    { if (!o_.expired())
        if (s_) o_.commit(s_);
        else    o_.abort(); }


  // Direct append: hopefully the content is valid η data
  ηo &operator<<(Sn<u8c> const &xs);


  ηo &name(Stc &s);
  ηo &k(Stvc &s);

  template<class U>
  ηo &v(U &&x) { return *this << std::forward<U>(x); }


  ηo &operator<<(int x) { return *this << Sc<i64>(x); }

  ηo &operator<<(i64 x);
  ηo &operator<<(f32 x);
  ηo &operator<<(f64 x);
  ηo &operator<<(Stc &s);
  ηo &operator<<(Stvc &s);

  ηo &operator<<(bool b);
  ηo &operator<<(ηatom a);
  ηo &operator<<(ηsig s);

  ηo &operator<<(ηic &i);
  ηo &operator<<(ηoc<B> &o);

  ηo &operator<<(Sn<i8bc>  const&);
  ηo &operator<<(Sn<i16bc> const&);
  ηo &operator<<(Sn<i32bc> const&);
  ηo &operator<<(Sn<i64bc> const&);
  ηo &operator<<(Sn<f32bc> const&);
  ηo &operator<<(Sn<f64bc> const&);

  ηo &operator<<(Sn<i8c>  const&);
  ηo &operator<<(Sn<i16c> const&);
  ηo &operator<<(Sn<i32c> const&);
  ηo &operator<<(Sn<i64c> const&);
  ηo &operator<<(Sn<f32c> const&);
  ηo &operator<<(Sn<f64c> const&);

  ηo &operator<<(V<i8>  const&);
  ηo &operator<<(V<i16> const&);
  ηo &operator<<(V<i32> const&);
  ηo &operator<<(V<i64> const&);
  ηo &operator<<(V<f32> const&);
  ηo &operator<<(V<f64> const&);


private:
  ηoc<T> o_;
  Sn<u8> b_;  // invariant: this points to memory managed by *o, or null
  uN     s_;  // current number of bytes written to the stream

  bool reserve(uN l);
};


}

#include "end.hh"

#endif
