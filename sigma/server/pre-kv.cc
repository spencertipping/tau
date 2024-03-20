#include "pre-db.hh"
#include "../pre-kv.hh"
#include "../../tau/begin.hh"
#include <lmdb.h>

namespace σ::pre
{


// A single LMDB database within a file.
struct kv_lmdb_ final : public virtual kv_
{
  kv_lmdb_(Sp<lmdb_db> mdb, Stc &db) : mdb_(mdb), db_(db) {}
  ~kv_lmdb_() { commit(); }

  void sync()   override { commit(); mdb_->sync(); }
  void commit() override { mdb_->commit(); }

  ηi   get(ηic &x)         override { return mdb_->get(db_, x); }
  bool has(ηic &k)         override { return mdb_->has(db_, k); }
  void del(ηic &k)         override {        mdb_->del(db_, k); }
  void set(ηic &k, ηic &v) override {        mdb_->set(db_, k, v); }


protected:
  Sp<lmdb_db> mdb_;
  Stc         db_;
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
