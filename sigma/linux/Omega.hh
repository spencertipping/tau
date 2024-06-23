#ifndef σserver_Ω_h
#define σserver_Ω_h

#define σserver_Ω_debug_instantiations 0

#include "Omega-io.hh"
#include "Omega-h.hh"

#include "../prof.hh"
#include "../begin.hh"

namespace σ
{


void ΓΩ(Γφ&);


struct Ωf final
{
  Ωf(τ::Stc&, bool rw = false);

  Ωf   &operator=(τ::ηic&);
  τ::ηm operator*() const;

  void  set(τ::ηic &x) { *this = x; }
  τ::ηm get()    const { return **this; }

  τ::u64 size             ()      const { return fd_->size(); }
  τ::u32 rev              ()      const { τ::u32b r; fd_->pread(&r, 4, 4); return r; }
  void   write_header_into(Ωfd f) const { f->pwrite("Ωf\0\0\0\0\0", 8, 0); }

protected:
  τ::Stc         f_;
  mutable Ωfd    fd_;
  mutable τ::u32 rev_;
  bool           rw_;
};

typedef Ωf const Ωfc;


struct Ωl final
{
  Ωl(τ::Stc&, bool rw = false);

  typedef τ::u64 key;

  key    operator<<(τ::ηic&);
  τ::ηm  operator[](key)   const;
  τ::u64 offset    (key k) const { return k + 4; }
  τ::u64 size      ()      const { return *τ::uap<τ::u64bc>(map_ + 8); }
  τ::u32 size      (key k) const
    { let s = size();
      A(map_.ensure(k + 4) && k + 4 <= s, "Ωl: key " << k << " beyond EOF " << s);
      return *τ::uap<τ::u32bc>(map_ + k); }

  key next(key k) const
    { let s = size();
      let e = offset(k) + size(k);
      return offset(e) <= s ? e : 0; }

  struct it final
  {
    key       x;
    Ωl const &l;
    bool  operator==(it const &i) const { return x == i.x; }
    τ::ηm operator* ()            const { return l[x]; }
    it   &operator++()                  { x = l.next(x); return *this; }
  };
  it begin() const { return {16, *this}; }
  it end()   const { return {0,  *this}; }

  void sync() const { fd_->fsync(); }

  void write_header(τ::u64);

protected:
  τ::Stc f_;
  Ωfd    fd_;
  bool   rw_;
  Ωfm    map_;
};

typedef Ωl const Ωlc;


struct Ωa final  // insert-only k/v store
{
  Ωa(τ::Stc&, bool rw = false);

  void  add(τ::ηic&, τ::ηic&);
  τ::ηm get(τ::ηic&) const;
  void  commit(bool fsync = false);

  Ωl                    &l()       { return l_; }
  Ωlc                   &l() const { return l_; }
  Ωh <τ::u64b, τ::u64b> &h()       { return h_; }
  Ωhc<τ::u64b, τ::u64b> &h() const { return h_; }

protected:
  Ωl                           l_;
  mutable Ωh<τ::u64b, τ::u64b> h_;  // hash(k) → Ωl key

  τ::Sp<measurement>
    prof_ladd_,
    prof_hadd_,
    prof_hget_,
    prof_commit_,
    prof_fsync_;
};

typedef Ωa const Ωac;


struct Ω1 final  // insert-only set
{
  Ω1(τ::Stc&, bool rw = false);

  τ::P<Ωl::key, bool> add   (τ::ηic&);
  τ::ηm               get   (Ωl::key k) { return l_[k]; }
  void                commit(bool fsync = false);

  Ωl                    &l()       { return l_; }
  Ωlc                   &l() const { return l_; }
  Ωh <τ::u64b, τ::u64b> &h()       { return h_; }
  Ωhc<τ::u64b, τ::u64b> &h() const { return h_; }

protected:
  Ωl                           l_;
  mutable Ωh<τ::u64b, τ::u64b> h_;  // hash(k) → Ωl key

  τ::Sp<measurement>
    prof_add_,
    prof_commit_,
    prof_fsync_;
};


template<Ωh_k K> struct Ωs_gen;

template<Ωh_k K, Ωh_l L>
struct Ωs final
{
  typedef K const Kc;
  typedef L const Lc;

  typedef Ωs_gen<K> G;

  Ωs(τ::Stc &path, bool rw = false) : h_(path, rw) {}

  void add   (τ::ηic &x, Lc &l)               { h_.add(G::gen(x), l); }
  void commit(bool fsync = false)             { h_.commit(fsync); }
  bool get   (τ::ηic &x, τ::Fc<bool(Lc&)> &f) { return h_.get(G::gen(x), f); }

  Ωh <K, L> &h()       { return h_; }
  Ωhc<K, L> &h() const { return h_; }

protected:
  Ωh<K, L> h_;
};

struct Ωs_h128k final  // effectively-exact 128-bit key, generated from SHA256
{
  τ::h128 x;
  τ::SO operator<=>(Ωs_h128k const &o) const = default;
  explicit operator τ::u64() const { return *τ::uap<τ::u64bc>(x.begin()); }
};

}


namespace std
{

Tn struct hash<σ::Ωs_h128k>
{
  τ::u64 operator()(σ::Ωs_h128k const &x) const { return Sc<τ::u64>(x); }
};

}


namespace σ
{

template<Ωh_k K> struct Ωs_gen {};
template<> struct Ωs_gen<τ::u32b>  { static τ::u32b  gen(τ::ηic &x) { return x.hash(); } };
template<> struct Ωs_gen<τ::u64b>  { static τ::u64b  gen(τ::ηic &x) { return x.hash(); } };
template<> struct Ωs_gen<Ωs_h128k> { static Ωs_h128k gen(τ::ηic &x)
  { using namespace τ;
    h128 r;
    let h = x.sha256();
    std::copy_n(h.begin(), 16, r.begin());
    return {r}; } };

Tt using Ωs32  = Ωs<τ::u32b,  T>;
Tt using Ωs64  = Ωs<τ::u64b,  T>;
Tt using Ωs128 = Ωs<Ωs_h128k, T>;


#if σserver_Ω_debug_instantiations
template struct Ωs<τ::u32b, τ::u64b>;
template struct Ωs<τ::u64b, τ::u64b>;
template struct Ωs<Ωs_h128k, τ::u64b>;
#endif

}

#include "../end.hh"

#endif
