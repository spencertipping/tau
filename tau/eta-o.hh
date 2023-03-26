#ifndef τηo_h
#define τηo_h

#include "xi.hh"
#include "eta-types.hh"
#include "eta-i.hh"

#include "begin.hh"

namespace τ
{


// Inline output writer for η values, in this case into a ξ.
//
// This object accepts a stream of values that may overflow the initial
// allocation. If this happens, the next stream operation may block, but
// your memory will be safe; that is, aside from blocking you don't need
// to care about the details of the underlying allocation.
struct ηo final
{
  ηo(ξ &o, uN c0 = 256) : o_(o), b_(o.iptr(c0)), s_(0) {}
  ~ηo() { if (s_) o_.commit(s_); }

  ηo &operator<<(i64 x)
    { let b = x > Nl<i32>::max() || x < Nl<i32>::min() ? 8
            : x > Nl<i16>::max() || x < Nl<i16>::min() ? 4
            : x > Nl<i8> ::max() || x < Nl<i8> ::min() ? 2 : 1;
      reserve(b + 1);
      s_ += ηcb(b_.subspan(s_), ηtype::n_int, b);
      switch (b)
      {
      case 1: *Rc<i8b*> (b_.data() + s_) = x; break;
      case 2: *Rc<i16b*>(b_.data() + s_) = x; break;
      case 4: *Rc<i32b*>(b_.data() + s_) = x; break;
      case 8: *Rc<i64b*>(b_.data() + s_) = x; break;
      }
      s_ += b;
      return *this; }

  ηo &operator<<(f32 x)
    { reserve(5);
      s_ += ηcb(b_.subspan(s_), ηtype::n_float, 4);
      *Rc<f32b*>(b_.data() + s_) = x;
      s_ += 4;
      return *this; }

  ηo &operator<<(f64 x)
    { reserve(9);
      s_ += ηcb(b_.subspan(s_), ηtype::n_float, 8);
      *Rc<f64b*>(b_.data() + s_) = x;
      s_ += 4;
      return *this; }

  ηo &operator<<(Stc &s)
    { reserve(s.size() + 1 + ηsb(s.size()));
      s_ += ηcb(b_.subspan(s_), ηtype::string, s.size());
      memcpy(b_.data() + s_, s.data(), s.size());
      s_ += s.size();
      return *this; }

  ηo &operator<<(Stvc &s)
    { reserve(s.size() + 1 + ηsb(s.size()));
      s_ += ηcb(b_.subspan(s_), ηtype::string, s.size());
      memcpy(b_.data() + s_, s.data(), s.size());
      s_ += s.size();
      return *this; }

  ηo &operator<<(bool b)
    { reserve(2);
      s_ += ηcb(b_.subspan(s_), ηtype::atom, 1);
      b_[s_++] = Sc<u8>(b ? ηatom::ηtrue : ηatom::ηfalse);
      return *this; }

  ηo &operator<<(ηatom a)
    { reserve(2);
      s_ += ηcb(b_.subspan(s_), ηtype::atom, 1);
      b_[s_++] = Sc<u8>(a);
      return *this; }

  ηo &operator<<(ηsig s)
    { reserve(2);
      s_ += ηcb(b_.subspan(s_), ηtype::sig, 1);
      b_[s_++] = Sc<u8>(s);
      return *this; }

  ηo &operator<<(ηi &i)
    { reserve(i.osize() + 1 + ηsb(i.osize()));
      s_ += ηcb(b_.subspan(s_), ηtype::η, i.osize());
      memcpy(b_.data() + s_, i.odata(), i.osize());
      s_ += i.osize();
      return *this; }

  ηo &operator<<(Sn<i8c> xs)
    { reserve(xs.size_bytes() + 1 + ηsb(xs.size_bytes()));
      s_ += ηcb(b_.subspan(s_), ηtype::int8s, xs.size_bytes());
      std::copy(xs.begin(), xs.end(), b_.data() + s_);
      s_ += xs.size_bytes();
      return *this; }

  ηo &operator<<(Sn<i16c> xs)
    { reserve(xs.size_bytes() + 1 + ηsb(xs.size_bytes()));
      s_ += ηcb(b_.subspan(s_), ηtype::int16s, xs.size_bytes());
      i16b *y = Rc<i16b*>(b_.data() + s_);
      for (let x : xs) *y++ = x;
      s_ += xs.size_bytes();
      return *this; }

  ηo &operator<<(Sn<i32c> xs)
    { reserve(xs.size_bytes() + 1 + ηsb(xs.size_bytes()));
      s_ += ηcb(b_.subspan(s_), ηtype::int32s, xs.size_bytes());
      i32b *y = Rc<i32b*>(b_.data() + s_);
      for (let x : xs) *y++ = x;
      s_ += xs.size_bytes();
      return *this; }

  ηo &operator<<(Sn<i64c> xs)
    { reserve(xs.size_bytes() + 1 + ηsb(xs.size_bytes()));
      s_ += ηcb(b_.subspan(s_), ηtype::int64s, xs.size_bytes());
      i64b *y = Rc<i64b*>(b_.data() + s_);
      for (let x : xs) *y++ = x;
      s_ += xs.size_bytes();
      return *this; }

  ηo &operator<<(Sn<f32c> xs)
    { reserve(xs.size_bytes() + 1 + ηsb(xs.size_bytes()));
      s_ += ηcb(b_.subspan(s_), ηtype::int32s, xs.size_bytes());
      f32b *y = Rc<f32b*>(b_.data() + s_);
      for (let x : xs) *y++ = x;
      s_ += xs.size_bytes();
      return *this; }

  ηo &operator<<(Sn<f64c> xs)
    { reserve(xs.size_bytes() + 1 + ηsb(xs.size_bytes()));
      s_ += ηcb(b_.subspan(s_), ηtype::int64s, xs.size_bytes());
      f64b *y = Rc<f64b*>(b_.data() + s_);
      for (let x : xs) *y++ = x;
      s_ += xs.size_bytes();
      return *this; }

  ηo &name(Stc &s)
    { reserve(s.size() + 1 + ηsb(s.size()));
      s_ += ηcb(b_.subspan(s_), ηtype::name, s.size());
      memcpy(b_.data() + s_, s.data(), s.size());
      s_ += s.size();
      return *this; }

  ηo &k(Stvc &s)
    { reserve(s.size() + 1 + ηsb(s.size()));
      s_ += ηcb(b_.subspan(s_), ηtype::name, s.size());
      memcpy(b_.data() + s_, s.data(), s.size());
      s_ += s.size();
      return *this; }

  template<class T>
  ηo &v(T &&x) { return *this << std::forward<T>(x); }


private:
  ξ     &o_;
  Sn<u8> b_;   // invariant: this points to memory managed by *o
  uN     s_;   // current number of bytes written to the stream

  void reserve(uN l)
    { if (s_ + l > b_.size_bytes())
      { // Abort this allocation and create a new one at twice the current size
        // or s_ + l, whichever is larger.
        u8 *b = new u8[s_];
        memcpy(b, b_.data(), s_);
        o_.abort();
        let s = std::max(s_ + l, b_.size_bytes() << 1);
        b_ = o_.iptr(s);
        memcpy(b_.data(), b, s_);
        delete[] b; } }
};


}

#include "end.hh"

#endif
