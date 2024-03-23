#ifndef σpre_linux_db_h
#define σpre_linux_db_h

#include <lmdb.h>
#include <sqlite3.h>

#include "../../tau.hh"
#include "../../tau/begin.hh"

namespace σ::pre
{

using namespace τ;


struct lmdb_db;


struct lmdb_db final
{
  struct reader final
  {
    uN       rev;
    MDB_txn *t;
  };


  lmdb_db(Stc &f, uN max_dbs = 64, uN mapsize = 1ull << 24);
  ~lmdb_db();

  void sync();
  void commit();

  ηi   get(Stc&, ηic &k);
  bool has(Stc&, ηic &k);
  void del(Stc&, ηic &k);
  void set(Stc&, ηic &k, ηic &v);

  MDB_env *env() const { return e_; }

  void reset();  // commit and close readers


protected:
  MDB_env           *e_ = nullptr;  // DB env
  MDB_txn           *w_ = nullptr;  // write transaction, or null
  Rmu                wm_;           // writer management mutex
  M<Th::id, reader>  r_;            // read transactions
  Smu                rm_;           // reader management mutex
  M<St, MDB_dbi>     d_;            // cache of DB tables
  Smu                dm_;           // DBI table management mutex
  uN                 us_ = 0;       // uncommitted data size
  At<uN>             re_ = 0;       // revision (to signal to readers)

  bool should_resize(uN n, f64 safety = 4.0) const;
  void reserve(uN);
  void close_readers();

  MDB_txn *rt();
  MDB_txn *wt();
  MDB_dbi  open(Stc&);
};


struct sqlite_db final
{
  sqlite3             *db;
  M<St, sqlite3_stmt*> ss;

  ~sqlite_db()
    { for (let &s : ss) sqlite3_finalize(s.second);
      sqlite3_close(db); }
};


Sp<lmdb_db>   lmdb_open(Stc&);
Sp<sqlite_db> sqlite_open(Stc&);



}

#include "../../tau/end.hh"

#endif
