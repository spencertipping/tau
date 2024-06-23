#ifndef τηow_h
#define τηow_h

#include "eta-o.hh"
#include "eta-m.hh"
#include "begin.hh"


namespace τ
{

// IMPORTANT: keep these functions in a header file; if we don't, then it
// becomes impossible to create new ηo<> instances (which π does for its
// internal heap IO, at least at the time of this comment).


Tt void mwrite(T const x, void *y)
{
  memcpy(y, &x, sizeof(x));
}


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
    mwrite(i64bc(x), b_.data() + s_);
    s_ += 8;
  }
  else if (x > Nl<i16>::max() || x < Nl<i16>::min())
  {
    if (!reserve(5)) return *this;
    s_ += ηcb(b_.subspan(s_), ηtype::n_int, 4);
    mwrite(i32b(x), b_.data() + s_);
    s_ += 4;
  }
  else if (x > Nl<i8>::max() || x < Nl<i8>::min())
  {
    if (!reserve(3)) return *this;
    s_ += ηcb(b_.subspan(s_), ηtype::n_int, 2);
    mwrite(i16b(x), b_.data() + s_);
    s_ += 2;
  }
  else
  {
    if (!reserve(2)) return *this;
    s_ += ηcb(b_.subspan(s_), ηtype::n_int, 1);
    mwrite(i8b(x), b_.data() + s_);
    s_ += 1;
  }
  return *this;
}


Tt ηo<T> &ηo<T>::operator<<(u64 x)
{
  if (x > Nl<i32>::max())
  {
    if (!reserve(9)) return *this;
    s_ += ηcb(b_.subspan(s_), ηtype::n_int, 8);
    mwrite(i64b(x), b_.data() + s_);
    s_ += 8;
  }
  else if (x > Nl<i16>::max())
  {
    if (!reserve(5)) return *this;
    s_ += ηcb(b_.subspan(s_), ηtype::n_int, 4);
    mwrite(i32b(x), b_.data() + s_);
    s_ += 4;
  }
  else if (x > Nl<i8>::max())
  {
    if (!reserve(3)) return *this;
    s_ += ηcb(b_.subspan(s_), ηtype::n_int, 2);
    mwrite(i16b(x), b_.data() + s_);
    s_ += 2;
  }
  else
  {
    if (!reserve(2)) return *this;
    s_ += ηcb(b_.subspan(s_), ηtype::n_int, 1);
    mwrite(i8b(x), b_.data() + s_);
    s_ += 1;
  }
  return *this;
}


Tt ηo<T> &ηo<T>::operator<<(f32 x)
{
  if (!reserve(5)) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::n_float, 4);
  mwrite(f32b(x), b_.data() + s_);
  s_ += 4;
  return *this;
}

Tt ηo<T> &ηo<T>::operator<<(f64 x)
{
  if (!reserve(9)) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::n_float, 8);
  mwrite(f64b(x), b_.data() + s_);
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


Tt ηo<T> &ηo<T>::operator<<(ηname const &n)
{
  let s = n.x.size();
  if (!reserve(s + 1 + ηsb(s))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::name, s);
  memcpy(b_.data() + s_, n.x.data(), s);
  s_ += s;
  return *this;
}

Tt ηo<T> &ηo<T>::operator<<(ηbin const &n)
{
  let s = n.x.size();
  if (!reserve(s + 1 + ηsb(s))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::binary, s);
  memcpy(b_.data() + s_, n.x.data(), s);
  s_ += s;
  return *this;
}

Tt ηo<T> &ηo<T>::operator<<(ηbinv const &n)
{
  let s = n.x.size();
  if (!reserve(s + 1 + ηsb(s))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::binary, s);
  memcpy(b_.data() + s_, n.x.data(), s);
  s_ += s;
  return *this;
}

Tt template<uN N> ηo<T> &ηo<T>::operator<<(Ar<u8, N> const &xs)
{
  if (!reserve(N + 1 + ηsb(N))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::binary, N);
  memcpy(b_.data() + s_, xs.data(), N);
  s_ += N;
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
  let sb = i.lsize();
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


Tt Tx ηo<T> &ηo<T>::operator<<(Vc<X> &xs)
{
  for (let &x : xs)
    if constexpr (is_tuple_<X>::value) *this << (ηm{} << x);
    else                               *this << x;
  return *this;
}


Tt Txy ηo<T> &ηo<T>::operator<<(Mc<X, Y> &xs)
{
  for (let &[k, v] : xs)
    if constexpr (is_tuple_<Y>::value) *this << k << (ηm{} << v);
    else                               *this << k << v;
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
  Tt ηo<T> &ηo<T>::operator<<(ηvec<De<st>> const &xs) \
  { let sb = xs.x.size() * sizeof(st);                \
    if (!reserve(sb + 1 + ηsb(sb))) return *this;     \
    s_ += ηcb(b_.subspan(s_), ηtype::ηt, sb);         \
    tt *y = Rc<tt*>(b_.data() + s_);                  \
    for (let x : xs.x) *y++ = x;                      \
    s_ += sb;                                         \
    return *this; }

  τηspan(i8c,  i8b,  int8s)
  τηspan(i16c, i16b, int16s)
  τηspan(i32c, i32b, int32s)
  τηspan(i64c, i64b, int64s)
  τηspan(f32c, f32b, float32s)
  τηspan(f64c, f64b, float64s)

#undef τηspan


}

#include "end.hh"

#endif // ETA-OW_H_
