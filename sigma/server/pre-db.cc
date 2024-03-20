#include <lmdb.h>

#include "pre-db.hh"
#include "../../tau/begin.hh"

namespace σ::pre
{


static M<St, Wp<lmdb_db>>   lmdbs;
static M<St, Wp<sqlite_db>> sqlites;


Sp<lmdb_db> lmdb_open(Stc &f)
{
  let i = lmdbs.find(f);
  if (i != lmdbs.end() && !i->second.expired()) return i->second.lock();

  let r = Sp<lmdb_db>(new lmdb_db(f));
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


}

#include "../../tau/end.hh"
