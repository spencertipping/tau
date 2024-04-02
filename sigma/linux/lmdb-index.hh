#ifndef σserver_lmdb_index_h
#define σserver_lmdb_index_h

#include "lmdb.hh"
#include "lmdb-sstream.hh"
#include "../prof.hh"

#include "../begin.hh"

namespace σ
{


// A scalable multimap that allows for large sets of values per key. Values per
// key are distinct on retrieval, and amortize to O(log n) insertion or better.
//
// We don't duplicate keys to store multiple values, so we pack multiple things
// into a single η stream within the backend database. We also guarantee that
// elements are returned in a deterministic order; we use ηsstream's ordering
// for this, which defaults to ηi::fast_compare.
//
// We insert two types of key/value pairs into the underlying k/v store:
//
// 1. Literal keys: values are stored inline
// 2. Indirect keys: the value is an η stream of keys that contain the actual
//    values
//
// Additionally, we insert a special key to track the ID of the next indirect
// key we plan to create. Here's an example of all this in action:
//
// ("l" "foo") → 0 1 2 3 4            # literal key "foo" with values 1 2 3 4
// ("l" "bar") → 2 (2 "k1") (2 "k2")  # indirect key
// ("i" "k1")  → 1 3
// ("i" "k2")  → 2 4
//
// We keep an in-memory stage so we aren't commiting things to the database too
// eagerly. This allows us to write a batch of values into an indirect key and
// avoid fragmentation in the process.
//
// Insertion, deletion, and retrieval can all kick off a single-key merge, which
// causes all indirect values to be flattened into a single list. This is done
// with η sorted streams, so it's fast and space-efficient.
//
// Indirect keys need some explanation. First, the literal value is a lookup
// table for all of the indirects: it starts with the number of indirect keys
// and each further entry is (n k), where n == db[k].len(). We store them this
// way because later balance the indirects by merging the smallest ones.
//
// In summary:
//
// ("l" "foo") → 3 (1 "k1") (4 "k2") (18 "k3")  ← 3 means "three indirects"
// ("i" "k1")  → "bar"                          ← 1 value here
// ("i" "k2")  → "a" "b" "c" "d"                ← 4 values here
// ("i" "k3")  → ...                            ← 18 values here
//
// A key is balanced when it has fewer than log2(largest indirect) indirects.
// We always merge the smallest indirects first.

struct lmdb_index final
{
  Txs using V  = τ::V<Xs...>;
  Txs using M  = τ::M<Xs...>;
  Tx  using So = τ::So<X>;

  using uN  = τ::uN;
  using iN  = τ::iN;
  using Stc = τ::Stc;
  using ηm  = τ::ηm;
  using ηmc = τ::ηmc;
  using ηic = τ::ηic;

  typedef τ::So<ηm, ηsstream_cmp> stage;

  typedef ηmc key;  // user-facing key
  typedef ηmc ind;  // key that we expect to be indirect

  lmdb_index(τe &te, lmdb &db,
             iN svo = 64ll << 20,  // staged value overflow (bytes)
             iN sko = 64ll << 10,  // staged key overflow (#keys)
             iN lvs = 8ll << 10)   // literal value size (bytes)
    : te_(te), db_{db}, ss_(0),
      svo_(svo), sko_(sko), lvs_(lvs),
      nk_((τ::isha2{} << (ηm{} << τ::epoch_seconds()))()),
      prof_add_            (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "add")),
      prof_del_            (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "del")),
      prof_get_            (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "get")),
      prof_commit_         (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "commit")),
      prof_commit_key_     (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "commit_key")),
      prof_ss_             (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "ss")),
      prof_balance_        (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "balance")),
      prof_touch_          (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "touch")),
      prof_stage_add_      (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "stage_add")),
      prof_stage_del_      (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "stage_del")),
      prof_unstage_add_    (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "unstage_add")),
      prof_unstage_del_    (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "unstage_del")),
      prof_kv_indirects_   (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "kv_indirects")),
      prof_kv_ind_asc_     (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "kv_ind_asc")),
      prof_new_indirect_   (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "new_indirect")),
      prof_make_indirect_  (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "make_indirect")),
      prof_add_kv_literal_ (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "add_kv_literal")),
      prof_add_kv_indirect_(measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "add_kv_indirect")),
      prof_genkey_         (measurement_for(ηm{} << "lmdb_index" << db.filename() << db.table() << "genkey"))
    {}

  ~lmdb_index() { commit(); }


  void add(key &k, ηic &v);
  void del(key &k, ηic &v);
  ηm   get(key &k, τ::uN limit = 0) const;


  // FIXME: add directly to literal if it is within size

  // FIXME: allow tuning

  void commit();
  void commit(key &k);

  ηsstream ss(key &k) const;  // unified view of values at key


protected:
  void commit_();        // assumes lock is acquired
  void commit_(key &k);  // assumes lock is acquired

  void touch();
  void balance(key &k);

  void stage_add  (key &k, ηic &v);
  void stage_add  (key &k, ηm &&v);
  void stage_del  (key &k, ηic &v);
  void stage_del  (key &k, ηm &&v);
  void unstage_add(key &k, ηic &v);
  void unstage_del(key &k, ηic &v);
  bool staged_add (key &k);
  bool staged_del (key &k);


  ηsstream ss_add_stage(key &k) const;
  ηsstream ss_del_stage(key &k) const;
  ηsstream ss_kv       (key &k) const;
  ηsstream ss_literal  (key &k) const;  // fetch literal values
  ηsstream ss_indirect (key &k) const;  // union all indirect keys
  ηsstream ss_indirect1(ind &k) const;  // retrieve single indirect key

  bool     kv_has      (key &k) const;
  τ::uN    kv_ilen     (key &k) const;  // #indirects, or 0 for literal
  V<ηm>    kv_indirects(key &k) const;  // indirect keys
  So<ηm>   kv_ind_asc  (key &k) const;  // (n k) pairs sorted by asc n


  ηm   new_indirect   (stage const &v);
  ηm   new_indirect   (ηic &v);
  void make_indirect  (key &k);

  void add_kv_literal (key &k, stage const &v);
  void add_kv_indirect(key &k, stage const &v);
  void add_kv         (key &k, stage const &v);

  void del_kv_literal (key &k, stage const &v);
  void del_kv_indirect(key &k, stage const &v);
  void del_kv         (key &k, stage const &v);
  void del_all        (key &k);         // delete all values at key


  ηm genkey();
  ηm lkey(ηic &k)      const { return ηm{} << "l" << k.all(); }
  ηm ikey(ηic &k)      const { return ηm{} << "i" << k.all(); }
  ηm ikey(τ::h256c &k) const { return ηm{} << "i" << k; }


  τe             &te_;
  lmdb           &db_;
  mutable τ::Smu  lock_;  // lock for all mutations
  M<ηm, stage>    add_;   // staged values to add
  M<ηm, stage>    del_;   // staged values to delete
  iN              ss_;    // staged size = ∑|v| in stage
  iN              svo_;   // staged value overflow (bytes)
  iN              sko_;   // staged key overflow (#keys)
  iN              lvs_;   // literal value size limit (bytes)
  τ::h256         nk_;    // seed for genkey()

  measurement &prof_add_,
    &prof_del_,
    &prof_get_,
    &prof_commit_,
    &prof_commit_key_,
    &prof_ss_,
    &prof_balance_,
    &prof_touch_,
    &prof_stage_add_,
    &prof_stage_del_,
    &prof_unstage_add_,
    &prof_unstage_del_,
    &prof_kv_indirects_,
    &prof_kv_ind_asc_,
    &prof_new_indirect_,
    &prof_make_indirect_,
    &prof_add_kv_literal_,
    &prof_add_kv_indirect_,
    &prof_genkey_;
};


}

#include "../tau/end.hh"

#endif
