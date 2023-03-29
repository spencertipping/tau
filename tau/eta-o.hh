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
//
// NOTE: we cannot hold a strong reference to the underlying ξ, so we
// must tolerate it being deallocated while this object exists. If that
// happens, writer methods will silently become no-ops.

// TODO: this should be a template so we can write to different types of
// containers
struct ηo final
{
  // TODO: remove p_() when we replace Wp with non-volatile
  ηo(Wp<ξ> o, uN c0 = 256) : o_(o), p_(o.lock().get()), s_(0)
    { A(c0, "ηo with no initial capacity");
      b_ = p_ ? p_->iptr(c0) : Sn<u8>{Sc<u8*>(nullptr), 0}; }

  ~ηo()
    { if (!o_.expired())
        if (s_) p_->commit(s_);
        else    p_->abort(); }


  // Direct append: hopefully the content is valid η data
  ηo &operator<<(Sn<u8c> const &xs)
    { if (!reserve(xs.size_bytes())) return *this;
      memcpy(b_.data() + s_, xs.data(), xs.size_bytes());
      s_ += xs.size_bytes();
      return *this; }


  ηo &operator<<(int x) { return *this << Sc<i64>(x); }
  ηo &operator<<(i64 x)
    { let b = x > Nl<i32>::max() || x < Nl<i32>::min() ? 8
            : x > Nl<i16>::max() || x < Nl<i16>::min() ? 4
            : x > Nl<i8> ::max() || x < Nl<i8> ::min() ? 2 : 1;
      if (!reserve(b + 1)) return *this;
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

  ηo &operator<<(Sn<i8c> const &xs)
    { if (!reserve(xs.size_bytes() + 1 + ηsb(xs.size_bytes()))) return *this;
      s_ += ηcb(b_.subspan(s_), ηtype::int8s, xs.size_bytes());
      std::copy(xs.begin(), xs.end(), b_.data() + s_);
      s_ += xs.size_bytes();
      return *this; }


#define τηspan(st, tt, ηt)                                              \
  ηo &operator<<(Sn<st> const &xs)                                      \
    { if (!reserve(xs.size_bytes() + 1 + ηsb(xs.size_bytes()))) return *this; \
      s_ += ηcb(b_.subspan(s_), ηtype::ηt, xs.size_bytes());            \
      tt *y = Rc<tt*>(b_.data() + s_);                                  \
      for (let x : xs) *y++ = x;                                        \
      s_ += xs.size_bytes();                                            \
      return *this; }

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

  template<class T>
  ηo &v(T &&x) { return *this << std::forward<T>(x); }


private:
  Wp<ξ>   o_;
  ξ      *p_;  // cached copy, since o_ referent will never change
  Sn<u8>  b_;  // invariant: this points to memory managed by *o, or null
  uN      s_;  // current number of bytes written to the stream

  bool reserve(uN l);
};


}

#include "end.hh"

#endif
