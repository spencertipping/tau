#include "pre-db.hh"
#include "../pre-kv.hh"
#include "../../tau/begin.hh"
#include <lmdb.h>

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
  void abort()           { if (w_) mdb_txn_abort(w_),  w_ = nullptr; }
  void sync()   override { close_reader(); commit(); mdb_env_sync(mdb_->e, 1); }
  void commit() override
    { int rc;
      if (w_)
      { A((rc = mdb_txn_commit(w_)) == MDB_SUCCESS,
          "mdb_txn_commit() failed: " << mdb_strerror(rc));
        w_ = nullptr; } }


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
      A(rc == MDB_SUCCESS, "mdb_get(" << x << ") failed: " << mdb_strerror(rc));
      return ηi{(u8c*)v.mv_data, v.mv_size}; }

  void set(ηic &k, ηic &v) override;

  void del(ηic &k) override
    { MDB_val k_{k.lsize(), (void*) k.ldata()};
      let t = w();
      let rc = mdb_del(t, dbi_, &k_, nullptr);
      A(rc == MDB_SUCCESS || rc == MDB_NOTFOUND, "mdb_del(" << k << ") failed: " << mdb_strerror(rc));
      commit(); }

  bool has(ηic &k) override
    { MDB_val k_{k.lsize(), (void*) k.ldata()};
      MDB_val v;
      let t = r();
      let rc = mdb_get(t, dbi_, &k_, &v);
      if (rc == MDB_NOTFOUND) return false;
      A(rc == MDB_SUCCESS, "mdb_get(" << k << ") failed: " << mdb_strerror(rc));
      return true; }


protected:
  Sp<lmdb_db> mdb_;
  Stc         db_;
  MDB_txn    *r_;
  MDB_txn    *w_;
  MDB_dbi     dbi_;
  Λcsw        csw_;
};


void kv_lmdb_::set(ηic &k, ηic &v)
{
  MDB_val k_{k.lsize(), (void*) k.ldata()};
  MDB_val v_{v.lsize(), (void*) v.ldata()};
  int rc;
  while (1)
  {
    auto t = w();
    if ((rc = mdb_put(t, dbi_, &k_, &v_, 0)) == MDB_SUCCESS)
      if ((rc = mdb_txn_commit(t)) == MDB_SUCCESS)
      {
        w_ = nullptr;
        return;
      }

    if (rc == MDB_MAP_FULL)
    {
      abort();
      MDB_envinfo i;
      mdb_env_info(mdb_->e, &i);
      mdb_env_set_mapsize(mdb_->e, i.me_mapsize * 2);
    }
    else
      A(0, "mdb_put(" << k << ", " << v << ") or commit failed: " << mdb_strerror(rc));
  }
}


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
