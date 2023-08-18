#ifndef σpre_kv_h
#define σpre_kv_h

#include "../tau.hh"
#include "../tau/begin.hh"

namespace σ::pre
{

using namespace τ;


// Virtualized key/value store interface. This is used for native, LMDB,
// SQLite, and other key/value stores.
struct kv_
{
  virtual ~kv_() {}

  virtual ηi   get(ηic&) = 0;
  virtual void set(ηic&, ηic&) = 0;
  virtual void del(ηic&) = 0;
  virtual bool has(ηic&) = 0;

  // Optional operations
  virtual void commit() {}
  virtual void sync() {}
};


Sp<kv_> kv_native();
Sp<kv_> kv_lmdb  (Stc &path, Stc &db);
Sp<kv_> kv_sqlite(Stc &path, Stc &table);


}

#include "../tau/end.hh"

#endif
