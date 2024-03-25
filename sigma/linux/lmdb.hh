#ifndef σserver_lmdb_h
#define σserver_lmdb_h

#include <lmdb.h>

#include "../prof.hh"
#include "../begin.hh"

namespace σ
{


void Γlmdb(Γφ&);


struct lmdb final
{
  using ηi    = τ::ηi;
  using ηic   = τ::ηic;
  using ηmc   = τ::ηmc;
  using ηm    = τ::ηm;
  Tt using Sp = τ::Sp<T>;

protected:
  struct rtx_;


public:
  // RAII value wrapper: holds the zero-copy value in place until deleted.
  Tt struct v final
  {
    T x;

    v(v<T> const&) = default;
    v(v<T> &&)     = default;
    v(T const &x, v<T> const &o) : x(x), rtx(o.rtx), stv(o.stv) {}
    v(T const &x = {}, Sp<rtx_> const &rtx = {}, Sp<ηm> const &stv = {})
      : x(x), rtx(rtx), stv(stv) {}

    v &operator=(v const&) = default;
    v &operator=(v&&)      = default;

    Tx auto map(X const &f) -> decltype(f(x)) { return v{f(x), *this}; }

  protected:
    // Only one of the two values below will be populated; it depends on where
    // the value resides.
    Sp<rtx_> rtx;
    Sp<ηm>   stv;
  };


  lmdb(τe &te, τ::Stc &f, τ::Stc &t,
       τ::uN mapsize = 1ull << 40,    // 1TiB by default
       τ::uN maxdbs  = 64,
       τ::uN mss     = 64ull << 20);  // 64MB max staged data
  ~lmdb();

  v<ηi> get(ηmc&) const;
  bool  has(ηmc&) const;
  void  set(ηmc&, ηic&);
  void  set(ηmc&, ηm&&);
  void  del(ηmc&);

  // Commit stage to DB, sync to disk if requested
  void commit(bool sync = false);

  MDB_env *env() const { return e_; }

  τ::Stc &filename() const { return f_; }
  τ::Stc &table()    const { return t_; }


protected:
  τe                 &te_;
  τ::Stc              f_;
  τ::Stc              t_;
  MDB_env            *e_;
  MDB_dbi             d_;
  mutable τ::Smu      rmu_;     // read transaction mutex
  mutable Sp<rtx_>    rt_;      // current read transaction, if any
  mutable τ::Smu      smu_;     // stage mutex
  τ::uN               ss_;      // stage size (protected by smu_)
  τ::M<ηm, Sp<ηm>>    istage_;  // insert/update stage (smu_)
  τ::S<ηm>            dstage_;  // delete stage (smu_)
  τ::uN               mss_;     // max stage size before auto-commit
  τ::Smu              cmu_;     // commit mutex (writers share, committer locks)
  Sp<τ::F<void(int)>> on_sig_;

  measurement &prof_get_outer_,
    &prof_get_inner_,
    &prof_has_outer_,
    &prof_has_inner_,
    &prof_del_staged_,
    &prof_set_staged_,
    &prof_commit_outer_,
    &prof_commit_write_,
    &prof_reader_;


  // Read transaction: aborts when deleted
  struct rtx_ final
  {
    rtx_(MDB_env *e);
    ~rtx_();
    MDB_txn *t;
  };


  void  maybe_commit();
  Sp<rtx_> reader() const;
  MDB_val val(ηic &x) const { return {x.lsize(), Cc<τ::u8*>(x.ldata())}; }
};


}

#include "../end.hh"


#endif
