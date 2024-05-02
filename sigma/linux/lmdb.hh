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
  // RAII value wrapper: holds the zero-copy value in place until deleted. T is
  // a transformed zero-copy accessor, e.g. ηi if the data is η.
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

    Tx auto map(X const &f) -> v<decltype(f(x))> { return v{f(x), *this}; }

  protected:
    // Only one of the two values below will be populated; it depends on where
    // the value resides.
    Sp<rtx_> rtx;
    Sp<ηm>   stv;
  };


  lmdb(τe &te, τ::Stc &f, τ::Stc &t,
       τ::uN mapsize = 1ull << 40,    // 1TiB by default
       τ::uN maxdbs  = 64,
       τ::uN mss     = 64ull << 20,   // 64MB max staged data
       τ::f64 rf     = 4.0);          // repack at 4x disk size
  ~lmdb();

  v<ηi> get(ηmc&) const;
  bool  has(ηmc&) const;
  void  set(ηmc&, ηic&);
  void  set(ηmc&, ηm&&);
  void  del(ηmc&);

  // NOTE: prefetch needs to be its own function because ηi looks at the first
  // few bytes of data to determine whether it overflows its container size.
  // That means we can't do a prefetch inside get() because we'd be blocking on
  // the data becoming available anyway.
  //
  // So the correct usage is, if you're pulling keys in bulk, to first
  // .prefetch() everything and then .get() the same keys.
  //
  // At some point I may try to make v<> handle prefetching by delaying the
  // value constructor, but I'm skeptical that's the best strategy.
  void prefetch(ηmc&) const;  // prefetch entry using madvise(MADV_WILLNEED)

  // Commit stage to DB, sync to disk if requested
  void  commit(bool sync = false);
  void  repack(bool sync = false);
  τ::uS disk_size() const;  // size of DB on disk

  Sp<MDB_env> env() const { return e_; }

  τ::Stc &filename() const { return f_; }
  τ::Stc &table()    const { return t_; }


protected:
  τe              &te_;
  τ::Stc           f_;
  τ::Stc           t_;
  Sp<MDB_env>      e_;
  MDB_dbi          d_;
  τ::uN            maxdbs_;      // saved so we can set it for repack
  mutable τ::Smu   rmu_;         // read transaction mutex
  mutable Sp<rtx_> rt_;          // current read transaction, if any
  mutable τ::Smu   smu_;         // stage mutex (readers/committer share)
  τ::uN            ss_;          // stage size (protected by smu_)
  τ::M<ηm, Sp<ηm>> istage_;      // insert/update stage (smu_)
  τ::S<ηm>         dstage_;      // delete stage (smu_)
  τ::uN            mss_;         // max stage size before auto-commit
  τ::Smu           cmu_;         // commit mutex (writers share, committer locks)
  τ::uN            dsize_;       // amount of deleted data since repack (cmu_)
  τ::uN            isize_;       // amount of expanded data since repack (cmu_)
  τ::uS            next_rep_;    // next repack size
  τ::f64           rep_factor_;  // repack at this * disk size

  measurement
    &prof_get_outer_,
    &prof_get_inner_,
    &prof_has_outer_,
    &prof_has_inner_,
    &prof_prefetch_outer_,
    &prof_prefetch_inner_,
    &prof_del_staged_,
    &prof_set_staged_,
    &prof_commit_outer_,
    &prof_commit_write_,
    &prof_reader_,
    &prof_repack_outer_,
    &prof_repack_inner_;


  // Read transaction: aborts when deleted
  struct rtx_ final
  {
    rtx_(Sp<MDB_env>);
    ~rtx_();
    Sp<MDB_env> e;
    MDB_txn    *t;
  };


  void maybe_commit(bool sync = false);
  void maybe_repack(bool sync = false);
  void commit_     (bool sync = false);  // NOTE: must be called with cmu_ held

  Sp<rtx_> reader() const;
  MDB_val  val(ηic &x) const { return {x.lsize(), Cc<τ::u8*>(x.ldata())}; }
};


}

#include "../end.hh"


#endif
