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
// NOTE: there may be more of these
Tt struct ηoc;

template<> struct ηoc<ξ> final
{
  void   commit(uN n) { wpg(o)->commit(n); }
  void   abort()      { wpg(o)->abort(); }
  bool   expired()    { return o.expired(); }
  Sn<u8> iptr(uN n)   { return wpg(o)->iptr(n); }

  Wp<ξ> o;
};

template<> struct ηoc<B&> final
{
  ηoc(B& b) : b(b), s(0) {}

  void   commit(uN n) { if (n) b.resize(s + n); s = 0; }
  void   abort()      {        b.resize(s);     s = 0; }
  bool   expired()    { return false; }
  Sn<u8> iptr(uN n)
    { A(!s, "ηoc<B&>::iptr(" << n << ") called with uncommitted data");
      b.resize((s = b.size()) + n);
      return {b.data() + s, n}; }

  B& b;
  uN s;  // last committed size
};


// Inline output writer for η values, for example into a ξ.
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

Tt struct ηo final
{
  ηo()          = delete;
  ηo(ηo const&) = delete;
  ηo(ηo&&)      = delete;

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

  // NOTE: these are synonymous, but named differently to help C++ figure out
  // which overload to use.
  ηo &name(Stc &s);
  ηo &k   (Stvc &s);

  template<class U>
  ηo &v(U &&x) { return *this << std::forward<U>(x); }


  template<class... Xs>
  ηo &operator<<(std::tuple<Xs...> const &xs)
    { std::apply([this](auto &&... xs) { (*this << ... << xs); }, xs);
      return *this; }


  ηo &operator<<(int x) { return *this << Sc<i64>(x); }

  ηo &operator<<(i64 x);
  ηo &operator<<(f32 x);
  ηo &operator<<(f64 x);
  ηo &operator<<(Stc &s);
  ηo &operator<<(Stvc &s);
  ηo &operator<<(chc *s);

  ηo &operator<<(bool b);
  ηo &operator<<(ηatom a);
  ηo &operator<<(ηsig s);

  ηo &operator<<(ηic &i);
  ηo &operator<<(ηoc<B&> &o);

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


// IMPORTANT: keep these functions in the header file; if we don't, then
// it becomes impossible to create new ηo<> instances (which π does for
// its internal heap IO, at least at the time of this comment).


Tt ηo<T> &ηo<T>::operator<<(Sn<u8c> const &xs)
{
  if (!reserve(xs.size_bytes())) return *this;
  memcpy(b_.data() + s_, xs.data(), xs.size_bytes());
  s_ += xs.size_bytes();
  return *this;
}


Tt ηo<T> &ηo<T>::operator<<(i64 x)
{
  if (x > Nl<i32>::max() || x < Nl<i32>::min())
  {
    if (!reserve(9)) return *this;
    s_ += ηcb(b_.subspan(s_), ηtype::n_int, 8);
    *Rc<i64b*>(b_.data() + s_) = x;
    s_ += 8;
  }
  else if (x > Nl<i16>::max() || x < Nl<i16>::min())
  {
    if (!reserve(5)) return *this;
    s_ += ηcb(b_.subspan(s_), ηtype::n_int, 4);
    *Rc<i32b*>(b_.data() + s_) = x;
    s_ += 4;
  }
  else if (x > Nl<i8>::max() || x < Nl<i8>::min())
  {
    if (!reserve(3)) return *this;
    s_ += ηcb(b_.subspan(s_), ηtype::n_int, 2);
    *Rc<i16b*>(b_.data() + s_) = x;
    s_ += 2;
  }
  else
  {
    if (!reserve(2)) return *this;
    s_ += ηcb(b_.subspan(s_), ηtype::n_int, 1);
    *Rc<i8b*>(b_.data() + s_) = x;
    s_ += 1;
  }
  return *this;
}


Tt ηo<T> &ηo<T>::operator<<(f32 x)
{
  if (!reserve(5)) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::n_float, 4);
  *Rc<f32b*>(b_.data() + s_) = x;
  s_ += 4;
  return *this;
}

Tt ηo<T> &ηo<T>::operator<<(f64 x)
{
  if (!reserve(9)) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::n_float, 8);
  *Rc<f64b*>(b_.data() + s_) = x;
  s_ += 8;
  return *this;
}

Tt ηo<T> &ηo<T>::operator<<(Stc &s)
{
  if (!reserve(s.size() + 1 + ηsb(s.size()))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::string, s.size());
  memcpy(b_.data() + s_, s.data(), s.size());
  s_ += s.size();
  return *this;
}

Tt ηo<T> &ηo<T>::operator<<(Stvc &s)
{
  if (!reserve(s.size() + 1 + ηsb(s.size()))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::string, s.size());
  memcpy(b_.data() + s_, s.data(), s.size());
  s_ += s.size();
  return *this;
}

Tt ηo<T> &ηo<T>::operator<<(chc *s)
{
  let n = strlen(s);
  if (!reserve(n + 1 + ηsb(n))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::string, n);
  memcpy(b_.data() + s_, s, n);
  s_ += n;
  return *this;
}


Tt ηo<T> &ηo<T>::operator<<(bool b)
{
  if (!reserve(2)) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::atom, 1);
  b_[s_++] = Sc<u8>(b ? ηatom::ηtrue : ηatom::ηfalse);
  return *this;
}

Tt ηo<T> &ηo<T>::operator<<(ηatom a)
{
  if (!reserve(2)) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::atom, 1);
  b_[s_++] = Sc<u8>(a);
  return *this;
}

Tt ηo<T> &ηo<T>::operator<<(ηsig s)
{
  if (!reserve(2)) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::sig, 1);
  b_[s_++] = Sc<u8>(s);
  return *this;
}


Tt ηo<T> &ηo<T>::operator<<(ηic &i)
{
  let sb = i.osize();
  if (!reserve(sb + 1 + ηsb(sb))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::η, sb);
  memcpy(b_.data() + s_, i.odata(), sb);
  s_ += sb;
  return *this;
}

Tt ηo<T> &ηo<T>::operator<<(ηoc<B&> &o)
{
  let sb = o.b.size();
  if (!reserve(sb + 1 + ηsb(sb))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::η, sb);
  memcpy(b_.data() + s_, o.b.data(), sb);
  s_ += sb;
  return *this;
}


Tt ηo<T> &ηo<T>::name(Stc &s)
{
  if (!reserve(s.size() + 1 + ηsb(s.size()))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::name, s.size());
  memcpy(b_.data() + s_, s.data(), s.size());
  s_ += s.size();
  return *this;
}

Tt ηo<T> &ηo<T>::k(Stvc &s)
{
  if (!reserve(s.size() + 1 + ηsb(s.size()))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::name, s.size());
  memcpy(b_.data() + s_, s.data(), s.size());
  s_ += s.size();
  return *this;
}


#define τηspan(st, tt, ηt)                            \
  Tt ηo<T> &ηo<T>::operator<<(Sn<tt const> const &xs) \
  { let sb = xs.size_bytes();                         \
    if (!reserve(sb + 1 + ηsb(sb))) return *this;     \
    s_ += ηcb(b_.subspan(s_), ηtype::ηt, sb);         \
    tt *y = Rc<tt*>(b_.data() + s_);                  \
    memcpy(y, xs.data(), sb);                         \
    s_ += sb;                                         \
    return *this; }                                   \
                                                      \
  Tt ηo<T> &ηo<T>::operator<<(Sn<st> const &xs)       \
  { let sb = xs.size_bytes();                         \
    if (!reserve(sb + 1 + ηsb(sb))) return *this;     \
    s_ += ηcb(b_.subspan(s_), ηtype::ηt, sb);         \
    tt *y = Rc<tt*>(b_.data() + s_);                  \
    for (let x : xs) *y++ = x;                        \
    s_ += sb;                                         \
    return *this; }                                   \
                                                      \
  Tt ηo<T> &ηo<T>::operator<<(V<De<st>> const &xs)    \
  { let sb = xs.size() * sizeof(st);                  \
    if (!reserve(sb + 1 + ηsb(sb))) return *this;     \
    s_ += ηcb(b_.subspan(s_), ηtype::ηt, sb);         \
    tt *y = Rc<tt*>(b_.data() + s_);                  \
    for (let x : xs) *y++ = x;                        \
    s_ += sb;                                         \
    return *this; }

  τηspan(i8c,  i8b,  int8s)
  τηspan(i16c, i16b, int16s)
  τηspan(i32c, i32b, int32s)
  τηspan(i64c, i64b, int64s)
  τηspan(f32c, f32b, float32s)
  τηspan(f64c, f64b, float64s)

#undef τηspan


Tt bool ηo<T>::reserve(uN l)
{
  if (o_.expired())
  {
    b_ = {Sc<u8*>(nullptr), 0};
    return false;
  }

  if (s_ + l > b_.size_bytes())
  {
    // Abort this allocation and create a new one at twice the current size
    // or s_ + l, whichever is larger.
    u8 *b = new u8[s_];
    memcpy(b, b_.data(), s_);
    o_.abort();
    let s = std::max(s_ + l, Sc<uN>(b_.size_bytes() << 1));

    // Complete the copy only if we actually have memory. ξ can be deallocated
    // during the iptr() call, in which case we'll get an empty span back.
    if (!(b_ = o_.iptr(s)).empty()) memcpy(b_.data(), b, s_);
    delete[] b;
  }

  return !b_.empty();
}


}

#include "end.hh"

#endif
