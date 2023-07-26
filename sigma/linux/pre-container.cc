#include "pre-container.hh"
#include "../begin.hh"
#include <lmdb.h>

namespace σ::pre
{


static M<St, Wp<lmdb_db>>   lmdbs;
static M<St, Wp<sqlite_db>> sqlites;


Sp<lmdb_db> lmdb_open(Stc &f)
{
  let i = lmdbs.find(f);
  if (i != lmdbs.end() && !i->second.expired()) return i->second.lock();

  let r = Sp<lmdb_db>(new lmdb_db);
  A(mdb_env_create(&r->e)                 == MDB_SUCCESS, "mdb_env_create() failed");
  A(mdb_env_set_mapsize(r->e, 1ull << 40) == MDB_SUCCESS, "mdb_env_set_mapsize() failed");
  A(mdb_env_open(r->e, f.c_str(), MDB_WRITEMAP | MDB_CREATE | MDB_NOSUBDIR, 0664) == MDB_SUCCESS,
    "mdb_env_open(" << f << ") failed");
  lmdbs[f] = r;
  return r;
}

Sp<sqlite_db> sqlite_open(Stc &db)
{
  let i = sqlites.find(db);
  if (i != sqlites.end() && !i->second.expired()) return i->second.lock();

  let r = Sp<sqlite_db>(new sqlite_db);
  A(sqlite3_open(db.c_str(), &r->db) == SQLITE_OK, "sqlite3_open(" << db << ") failed");
  sqlites[db] = r;
  return r;
}


struct lmdb_at_
{
  Sp<lmdb_db> l_;
  MDB_txn    *w_;
  Λcsw        c_;

  lmdb_at_(cb_lmdb const &l)
    : l_(lmdb_open(l.f)),
      w_(nullptr),
      c_([this]() { commit(); }) {}

  virtual ~lmdb_at_() { commit(); }

  MDB_txn *txn()
    { if (!w_) mdb_txn_begin(l_->e, nullptr, 0, &w_);
      return w_; }

  void commit()
    { if (w_) mdb_txn_commit(w_);
      w_ = nullptr; }

  MDB_dbi dbi(Stc &db)
    { MDB_dbi r; mdb_dbi_open(txn(), db.c_str(), MDB_CREATE, &r); return r; }

  Sn<u8c> get(Stc &db, Sn<u8c> const &k)
    { std::cerr << "get(" << db << ")" << std::endl;
      let d = dbi(db);
      MDB_val key = {k.size(), (void*)k.data()};
      MDB_val val;
      let r = mdb_get(txn(), d, &key, &val);
      if (r == MDB_NOTFOUND) return {};
      return {Rc<u8c*>(val.mv_data), val.mv_size}; }

  void set(Stc &db, Sn<u8c> const &k, Sn<u8c> const &v)
    { let d = dbi(db);
      MDB_val key = {k.size(), (void*)k.data()};
      MDB_val val = {v.size(), (void*)v.data()};
      let rc = mdb_put(txn(), d, &key, &val, 0);
      A(rc == MDB_SUCCESS, "mdb_put() failed: " << mdb_strerror(rc)); }

  void del(Stc &db, Sn<u8c> const &k)
    { let d = dbi(db);
      MDB_val key = {k.size(), (void*)k.data()};
      let rc = mdb_del(txn(), d, &key, nullptr);
      A(rc == MDB_SUCCESS, "mdb_del() failed: " << mdb_strerror(rc)); }
};


struct lsat_ : public virtual at_, public virtual lmdb_at_
{
  lsat_(cb_lmdb const &l) : at_(ct_set{}, l), lmdb_at_(l), db(l.db) {}

  void α(ηic &x, ξo)   override { set(db, x.one().all(), {&m, 1}); }
  void ω(ηic &x, ξo)   override { del(db, x.one().all()); }
  void ι(ηic &x, ξo o) override { o.r(x.lsize() + 2) << x.all() << !get(db, x.one().all()).empty(); }

  St db;

  static constexpr u8 m = 1;
};


struct lmat_ : public virtual at_, public virtual lmdb_at_
{
  lmat_(cb_lmdb const &l) : at_(ct_map{}, l), lmdb_at_(l), db(l.db) {}
  void α(ηic &x, ξo)   override { set(db, x.one().all(), x.next().all()); }
  void ω(ηic &x, ξo)   override { del(db, x.one().all()); }
  void ι(ηic &x, ξo o) override
    { let r = get(db, x.one().all());
      if (!r.empty()) o.r(x.lsize() + r.size() + 8) << x.all() << r;
      else            o.r(x.lsize() + 2)            << x.all() << r; }

  St db;
};


Sp<at_> lmdb_set(cb_lmdb const &l)
{
  return Sp<at_>(new lsat_(l));
}

Sp<at_> lmdb_map(cb_lmdb const &l)
{
  return Sp<at_>(new lmat_(l));
}


}

#include "../end.hh"
