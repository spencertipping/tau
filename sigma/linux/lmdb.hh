#ifndef σserver_lmdb_h
#define σserver_lmdb_h

#include <lmdb.h>

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
    v(T const &x = {}, Sp<rtx_> const &rtx = {}, Sp<ηm> const &stv = {})
      : x(x), rtx(rtx), stv(stv) {}

    v(T const &x, v<T> const &o) : x(x), rtx(o.rtx), stv(o.stv) {}

    v(v<T> const&) = default;
    v(v<T> &&)     = default;

    T x;

    Tx auto map(X const &f) -> decltype(f(x))
      { return v{f(x), *this}; }

  protected:
    // Only one of the two values below will be populated; it depends on where
    // the value resides.
    Sp<rtx_> rtx;
    Sp<ηm>   stv;
  };


  lmdb(τ::Stc &f, τ::Stc &t,
       τ::uN mapsize = 1ull << 24,
       τ::uN maxdbs  = 64,
       τ::uN mss     = 64ull << 20);
  ~lmdb();

  v<ηi> get(ηmc&) const;
  bool  has(ηmc&) const;
  void  set(ηmc&, ηic&);
  void  set(ηmc&, ηm&&);
  void  del(ηmc&);

  // Commit stage to DB, sync to disk if requested
  void commit(bool sync = false);

  MDB_env *env() const { return e_; }


protected:
  MDB_env          *e_;
  MDB_dbi           d_;
  mutable τ::Smu    rmu_;     // read transaction mutex
  mutable Sp<rtx_>  rt_;      // current read transaction, if any
  mutable τ::Smu    smu_;     // stage mutex
  τ::uN             ss_;      // stage size (protected by smu_)
  τ::M<ηm, Sp<ηm>>  istage_;  // insert/update stage (smu_)
  τ::S<ηm>          dstage_;  // delete stage (smu_)
  τ::uN             mss_;     // max stage size before auto-commit
  τ::Smu            cmu_;     // commit mutex (writers share, committer locks)

  // Read transaction: aborts when deleted
  struct rtx_ final
  {
    rtx_(MDB_env *e);
    ~rtx_();
    MDB_txn *t;
  };


  // Returns true if we should resize the database before writing n more bytes
  // of data into it.
  bool should_resize(τ::uN n, τ::f64 safety_factor = 4.0) const;

  void  maybe_commit();
  Sp<rtx_> reader() const;
  MDB_val val(ηic &x) const { return {x.lsize(), Cc<τ::u8*>(x.ldata())}; }
};


}

#include "../end.hh"


#endif
