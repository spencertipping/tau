#include "pre-db.hh"
#include "../pre-kv.hh"
#include "../../tau/begin.hh"

namespace σ::pre
{


// A single LMDB database within a file.
struct kv_lmdb_ final : public virtual kv_
{
  kv_lmdb_(Sp<lmdb_db> mdb, Stc &db)
    : mdb_(mdb), db_(db), r_(nullptr), w_(nullptr), dbi_(0),
      csw_([this]() { commit(); close_reader(); })
    {}

  ~kv_lmdb_() { commit(); close_reader(); }


  void close_reader()    { if (r_) mdb_txn_abort(r_),  r_ = nullptr; }
  void commit() override { if (w_) mdb_txn_commit(w_), w_ = nullptr; }
  void abort()           { if (w_) mdb_txn_abort(w_),  w_ = nullptr; }
  void sync()   override { close_reader(); commit(); mdb_env_sync(mdb_->e, 1); }


  MDB_txn *r()
    { commit();
      if (!r_)
      { let rc = mdb_txn_begin(mdb_->e, nullptr, MDB_RDONLY, &r_);
        A(rc == MDB_SUCCESS, "r::mdb_txn_begin() failed: " << mdb_strerror(rc));
        if (mdb_dbi_open(r_, db_.c_str(), MDB_CREATE, &dbi_) != MDB_SUCCESS)
        { w(); return r(); } }
      return r_; }

  MDB_txn *w()
    { close_reader();
      if (!w_)
      { int rc;
        A((rc = mdb_txn_begin(mdb_->e, nullptr, 0, &w_)) == MDB_SUCCESS,
          "w::mdb_txn_begin() failed: " << mdb_strerror(rc));
        A((rc = mdb_dbi_open(w_, db_.c_str(), MDB_CREATE, &dbi_)) == MDB_SUCCESS,
          "w::mdb_dbi_open() failed: " << mdb_strerror(rc)); }
      return w_; }


  ηi get(ηic &x) override
    { MDB_val k{x.lsize(), (void *)x.ldata()};
      MDB_val v;
      let t = r();
      let rc = mdb_get(t, dbi_, &k, &v);
      if (rc == MDB_NOTFOUND) return ηi{};
      A(rc == MDB_SUCCESS, "mdb_get() failed: " << mdb_strerror(rc));
      return ηi{(u8c*)v.mv_data, v.mv_size}; }

  void set(ηic &k, ηic &v) override
    { MDB_val k_{k.lsize(), (void *)k.ldata()};
      MDB_val v_{v.lsize(), (void *)v.ldata()};
      auto t = w();
      mdb_del(t, dbi_, &k_, nullptr);
      auto rc = mdb_put(t, dbi_, &k_, &v_, 0);
      while (rc == MDB_MAP_FULL)
      { abort();
        MDB_envinfo i;
        mdb_env_info(mdb_->e, &i);
        mdb_env_set_mapsize(mdb_->e, i.me_mapsize * 2);
        t  = w();
        rc = mdb_put(t, dbi_, &k_, &v_, 0); }
      A(rc == MDB_SUCCESS, "mdb_put() failed: " << mdb_strerror(rc)); }

  void del(ηic &k) override
    { MDB_val k_{k.lsize(), (void *)k.ldata()};
      let t = w();
      let rc = mdb_del(t, dbi_, &k_, nullptr);
      A(rc == MDB_SUCCESS, "mdb_del() failed: " << mdb_strerror(rc)); }

  bool has(ηic &k) override
    { MDB_val k_{k.lsize(), (void *)k.ldata()};
      MDB_val v;
      let t = r();
      let rc = mdb_get(t, dbi_, &k_, &v);
      if (rc == MDB_NOTFOUND) return false;
      A(rc == MDB_SUCCESS, "mdb_get() failed: " << mdb_strerror(rc));
      return true; }


protected:
  Sp<lmdb_db> mdb_;
  Stc         db_;
  MDB_txn    *r_;
  MDB_txn    *w_;
  MDB_dbi     dbi_;
  Λcsw        csw_;
};


Sp<kv_> kv_lmdb(Stc &path, Stc &db)
{
  return Sp<kv_>{new kv_lmdb_(lmdb_open(path), db)};
}


Sp<kv_> kv_sqlite(Stc &path, Stc &db)
{
  TODO("implement kv_sqlite");
  return {};
}


}

#include "../../tau/end.hh"
