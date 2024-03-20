#include "pre-db.hh"
#include "../pre-container-kv.hh"
#include "../begin.hh"

namespace σ::pre
{


struct lmdb_ls_ : public virtual at_ls_
{
  lmdb_ls_(cb_lmdb const &l, Sp<lmdb_db> mdb) : at_ls_(l), mdb(mdb), db(l.db) {}

  void ls(ξo o)
    { Mp<MDB_txn> r(mdb_txn_abort);
      MDB_dbi     dbi;
      int         rc;

      A((rc = mdb_txn_begin(mdb->env(), nullptr, MDB_RDONLY, &r)) == MDB_SUCCESS,
        "r::mdb_txn_begin() failed: " << mdb_strerror(rc));
      A((rc = mdb_dbi_open(r, db.c_str(), MDB_CREATE, &dbi)) == MDB_SUCCESS,
        "r::mdb_dbi_open() failed: " << mdb_strerror(rc));

      Mp<MDB_cursor> c(mdb_cursor_close);
      A((rc = mdb_cursor_open(r, dbi, &c)) == MDB_SUCCESS,
        "mdb_cursor_open() failed: " << mdb_strerror(rc));
      MDB_val k, v;
      while ((rc = mdb_cursor_get(c, &k, &v, MDB_NEXT)) == MDB_SUCCESS)
        o.r(k.mv_size + v.mv_size + 8)
          << ηi{Rc<u8c*>(k.mv_data), k.mv_size}
          << Sn<u8c>{Rc<u8c*>(v.mv_data), v.mv_size};
      A(rc == MDB_NOTFOUND, "mdb_cursor_get() failed: " << mdb_strerror(rc)); }

  Sp<lmdb_db> mdb;
  St          db;
};


Sp<at_> lmdb_set(cb_lmdb const &l)
{
  return Sp<at_>(new kvsat_(l, kv_lmdb(l.f, l.db)));
}

Sp<at_> lmdb_map(cb_lmdb const &l)
{
  return Sp<at_>(new kvmat_(l, kv_lmdb(l.f, l.db)));
}

Sp<at_> lmdb_multimap(ct_multimap const &m, cb_lmdb const &l)
{
  return Sp<at_>(new kvmmat_(m, l, kv_lmdb(l.f, l.db)));
}


Sp<at_ls_> lmdb_ls(cb_lmdb const &l)
{
  return Sp<at_ls_>(new lmdb_ls_(l, lmdb_open(l.f)));
}


}

#include "../end.hh"
