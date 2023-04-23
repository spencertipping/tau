#include "eta-o.hh"
#include "begin.hh"

namespace τ
{


template<class T>
ηo<T> &ηo<T>::operator<<(Sn<u8c> const &xs)
{
  if (!reserve(xs.size_bytes())) return *this;
  memcpy(b_.data() + s_, xs.data(), xs.size_bytes());
  s_ += xs.size_bytes();
  return *this;
}


template<class T>
ηo<T> &ηo<T>::operator<<(i64 x)
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


template<class T>
ηo<T> &ηo<T>::operator<<(f32 x)
{
  if (!reserve(5)) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::n_float, 4);
  s_ += sizeof(*Rc<f32b*>(b_.data() + s_) = x);
  return *this;
}

template<class T>
ηo<T> &ηo<T>::operator<<(f64 x)
{
  if (!reserve(9)) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::n_float, 8);
  s_ += sizeof(*Rc<f64b*>(b_.data() + s_) = x);
  return *this;
}

template<class T>
ηo<T> &ηo<T>::operator<<(Stc &s)
{
  if (!reserve(s.size() + 1 + ηsb(s.size()))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::string, s.size());
  memcpy(b_.data() + s_, s.data(), s.size());
  s_ += s.size();
  return *this;
}

template<class T>
ηo<T> &ηo<T>::operator<<(Stvc &s)
{
  if (!reserve(s.size() + 1 + ηsb(s.size()))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::string, s.size());
  memcpy(b_.data() + s_, s.data(), s.size());
  s_ += s.size();
  return *this;
}

template<class T>
ηo<T> &ηo<T>::operator<<(chc *s)
{
  let n = strlen(s);
  if (!reserve(n + 1 + ηsb(n))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::string, n);
  memcpy(b_.data() + s_, s, n);
  s_ += n;
  return *this;
}


template<class T>
ηo<T> &ηo<T>::operator<<(bool b)
{
  if (!reserve(2)) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::atom, 1);
  b_[s_++] = Sc<u8>(b ? ηatom::ηtrue : ηatom::ηfalse);
  return *this;
}

template<class T>
ηo<T> &ηo<T>::operator<<(ηatom a)
{
  if (!reserve(2)) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::atom, 1);
  b_[s_++] = Sc<u8>(a);
  return *this;
}

template<class T>
ηo<T> &ηo<T>::operator<<(ηsig s)
{
  if (!reserve(2)) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::sig, 1);
  b_[s_++] = Sc<u8>(s);
  return *this;
}


template<class T>
ηo<T> &ηo<T>::operator<<(ηic &i)
{
  let sb = i.osize();
  if (!reserve(sb + 1 + ηsb(sb))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::η, sb);
  memcpy(b_.data() + s_, i.odata(), sb);
  s_ += sb;
  return *this;
}

template<class T>
ηo<T> &ηo<T>::operator<<(ηoc<B&> &o)
{
  let sb = o.b.size();
  if (!reserve(sb + 1 + ηsb(sb))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::η, sb);
  memcpy(b_.data() + s_, o.b.data(), sb);
  s_ += sb;
  return *this;
}


template<class T>
ηo<T> &ηo<T>::name(Stc &s)
{
  if (!reserve(s.size() + 1 + ηsb(s.size()))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::name, s.size());
  memcpy(b_.data() + s_, s.data(), s.size());
  s_ += s.size();
  return *this;
}

template<class T>
ηo<T> &ηo<T>::k(Stvc &s)
{
  if (!reserve(s.size() + 1 + ηsb(s.size()))) return *this;
  s_ += ηcb(b_.subspan(s_), ηtype::name, s.size());
  memcpy(b_.data() + s_, s.data(), s.size());
  s_ += s.size();
  return *this;
}


#define τηspan(st, tt, ηt)                            \
  template<class T>                                   \
  ηo<T> &ηo<T>::operator<<(Sn<tt const> const &xs)    \
  { let sb = xs.size_bytes();                         \
    if (!reserve(sb + 1 + ηsb(sb))) return *this;     \
    s_ += ηcb(b_.subspan(s_), ηtype::ηt, sb);         \
    tt *y = Rc<tt*>(b_.data() + s_);                  \
    memcpy(y, xs.data(), sb);                         \
    s_ += sb;                                         \
    return *this; }                                   \
                                                      \
  template<class T>                                   \
  ηo<T> &ηo<T>::operator<<(Sn<st> const &xs)          \
  { let sb = xs.size_bytes();                         \
    if (!reserve(sb + 1 + ηsb(sb))) return *this;     \
    s_ += ηcb(b_.subspan(s_), ηtype::ηt, sb);         \
    tt *y = Rc<tt*>(b_.data() + s_);                  \
    for (let x : xs) *y++ = x;                        \
    s_ += sb;                                         \
    return *this; }                                   \
                                                      \
  template<class T>                                   \
  ηo<T> &ηo<T>::operator<<(V<De<st>> const &xs)       \
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


template<class T>
bool ηo<T>::reserve(uN l)
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


template struct ηo<ξ>;
template struct ηo<B&>;


}

#include "end.hh"
