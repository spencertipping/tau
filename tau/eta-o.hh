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
  ηo(Λ &l, Wp<T> o, uN c0 = 256) : o_(o), s_(0)
    { A(c0, "ηo with no initial capacity");
      if (!o_.expired()) b_ = wpg(o_)->iptr(c0);
      else               l.x(l.i()), τunreachable(); }

  ~ηo()
    { if (!o_.expired())
        if (s_) wpg(o_)->commit(s_);
        else    wpg(o_)->abort(); }


  // Direct append: hopefully the content is valid η data
  ηo &operator<<(Sn<u8c> const &xs)
    { if (!reserve(xs.size_bytes())) return *this;
      memcpy(b_.data() + s_, xs.data(), xs.size_bytes());
      s_ += xs.size_bytes();
      return *this; }


  ηo &operator<<(int x) { return *this << Sc<i64>(x); }

  ηo &operator<<(i64 x)
    { if (x > Nl<i32>::max() || x < Nl<i32>::min())
      { if (!reserve(9)) return *this;
        s_ += ηcb(b_.subspan(s_), ηtype::n_int, 8);
        *Rc<i64b*>(b_.data() + s_) = x;
        s_ += 8; }
      else if (x > Nl<i16>::max() || x < Nl<i16>::min())
      { if (!reserve(5)) return *this;
        s_ += ηcb(b_.subspan(s_), ηtype::n_int, 4);
        *Rc<i32b*>(b_.data() + s_) = x;
        s_ += 4; }
      else if (x > Nl<i8>::max() || x < Nl<i8>::min())
      { if (!reserve(3)) return *this;
        s_ += ηcb(b_.subspan(s_), ηtype::n_int, 2);
        *Rc<i16b*>(b_.data() + s_) = x;
        s_ += 2; }
      else
      { if (!reserve(2)) return *this;
        s_ += ηcb(b_.subspan(s_), ηtype::n_int, 1);
        *Rc<i8b*>(b_.data() + s_) = x;
        s_ += 1; }
      return *this; }

  ηo &operator<<(f32 x)
    { if (!reserve(5)) return *this;
      s_ += ηcb(b_.subspan(s_), ηtype::n_float, 4);
      s_ += sizeof(*Rc<f32b*>(b_.data() + s_) = x);
      return *this; }

  ηo &operator<<(f64 x)
    { if (!reserve(9)) return *this;
      s_ += ηcb(b_.subspan(s_), ηtype::n_float, 8);
      s_ += sizeof(*Rc<f64b*>(b_.data() + s_) = x);
      return *this; }

  ηo &operator<<(Stc &s)
    { if (!reserve(s.size() + 1 + ηsb(s.size()))) return *this;
      s_ += ηcb(b_.subspan(s_), ηtype::string, s.size());
      memcpy(b_.data() + s_, s.data(), s.size());
      s_ += s.size();
      return *this; }

  ηo &operator<<(Stvc &s)
    { if (!reserve(s.size() + 1 + ηsb(s.size()))) return *this;
      s_ += ηcb(b_.subspan(s_), ηtype::string, s.size());
      memcpy(b_.data() + s_, s.data(), s.size());
      s_ += s.size();
      return *this; }

  ηo &operator<<(bool b)
    { if (!reserve(2)) return *this;
      s_ += ηcb(b_.subspan(s_), ηtype::atom, 1);
      b_[s_++] = Sc<u8>(b ? ηatom::ηtrue : ηatom::ηfalse);
      return *this; }

  ηo &operator<<(ηatom a)
    { if (!reserve(2)) return *this;
      s_ += ηcb(b_.subspan(s_), ηtype::atom, 1);
      b_[s_++] = Sc<u8>(a);
      return *this; }

  ηo &operator<<(ηsig s)
    { if (!reserve(2)) return *this;
      s_ += ηcb(b_.subspan(s_), ηtype::sig, 1);
      b_[s_++] = Sc<u8>(s);
      return *this; }

  ηo &operator<<(ηi &i)
    { if (!reserve(i.osize() + 1 + ηsb(i.osize()))) return *this;
      s_ += ηcb(b_.subspan(s_), ηtype::η, i.osize());
      memcpy(b_.data() + s_, i.odata(), i.osize());
      s_ += i.osize();
      return *this; }


#define τηspan(st, tt, ηt)                                              \
  ηo &operator<<(Sn<st> const &xs)                                      \
    { let sb = xs.size_bytes();                                         \
      if (!reserve(sb + 1 + ηsb(sb))) return *this;                     \
      s_ += ηcb(b_.subspan(s_), ηtype::ηt, sb);                         \
      tt *y = Rc<tt*>(b_.data() + s_);                                  \
      for (let x : xs) *y++ = x;                                        \
      s_ += sb;                                                         \
      return *this; }                                                   \
                                                                        \
  ηo &operator<<(V<De<st>> const &xs)                                   \
    { let sb = xs.size() * sizeof(st);                                  \
      if (!reserve(sb + 1 + ηsb(sb))) return *this;                     \
      s_ += ηcb(b_.subspan(s_), ηtype::ηt, sb);                         \
      tt *y = Rc<tt*>(b_.data() + s_);                                  \
      for (let x : xs) *y++ = x;                                        \
      s_ += sb;                                                         \
      return *this; }                                                   \

  τηspan(i8c,  i8b,  int8s)
  τηspan(i16c, i16b, int16s)
  τηspan(i32c, i32b, int32s)
  τηspan(i64c, i64b, int64s)
  τηspan(f32c, f32b, float32s)
  τηspan(f64c, f64b, float64s)

#undef τηspan


  ηo &name(Stc &s)
    { if (!reserve(s.size() + 1 + ηsb(s.size()))) return *this;
      s_ += ηcb(b_.subspan(s_), ηtype::name, s.size());
      memcpy(b_.data() + s_, s.data(), s.size());
      s_ += s.size();
      return *this; }

  ηo &k(Stvc &s)
    { if (!reserve(s.size() + 1 + ηsb(s.size()))) return *this;
      s_ += ηcb(b_.subspan(s_), ηtype::name, s.size());
      memcpy(b_.data() + s_, s.data(), s.size());
      s_ += s.size();
      return *this; }

  template<class U>
  ηo &v(U &&x) { return *this << std::forward<U>(x); }


private:
  Wp<T>  o_;
  Sn<u8> b_;  // invariant: this points to memory managed by *o, or null
  uN     s_;  // current number of bytes written to the stream

  bool reserve(uN l)
    { if (o_.expired())
      { b_ = {Sc<u8*>(nullptr), 0};
        return false; }

      if (s_ + l > b_.size_bytes())
      { // Abort this allocation and create a new one at twice the current size
        // or s_ + l, whichever is larger.
        u8 *b = new u8[s_];
        memcpy(b, b_.data(), s_);
        wpg(o_)->abort();
        let s = std::max(s_ + l, Sc<uN>(b_.size_bytes() << 1));

        // Complete the copy only if we actually have memory. ξ can be deallocated
        // during the iptr() call, in which case we'll get an empty span back.
        if (!(b_ = wpg(o_)->iptr(s)).empty()) memcpy(b_.data(), b, s_);
        delete[] b; }

      return !b_.empty(); }
};


}

#include "end.hh"

#endif
