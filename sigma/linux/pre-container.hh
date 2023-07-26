#ifndef σpre_linux_container_h
#define σpre_linux_container_h

#include <lmdb.h>
#include <sqlite3.h>

#include "../pre-container.hh"
#include "../begin.hh"

namespace σ::pre
{

using namespace τ;


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

#include "../end.hh"

#endif
