#ifndef σpre_linux_db_h
#define σpre_linux_db_h

#include <lmdb.h>
#include <sqlite3.h>

#include "../../tau.hh"
#include "../../tau/begin.hh"

namespace σ::pre
{

using namespace τ;


// FIXME: lmdb_db needs to track the current write transaction and commit
// it when the map is resized. The transaction is currently owned by kv_,
// which causes problems when multiple kv_ use the same database in a non-
// τ context.

struct lmdb_db final
{
  MDB_env *e;
  ~lmdb_db() { mdb_env_close(e); }
};

struct sqlite_db final
{
  sqlite3             *db;
  M<St, sqlite3_stmt*> ss;

  ~sqlite_db()
    { for (auto& s : ss) sqlite3_finalize(s.second);
      sqlite3_close(db); }
};


Sp<lmdb_db>   lmdb_open(Stc&);
Sp<sqlite_db> sqlite_open(Stc&);



}

#include "../../tau/end.hh"

#endif
