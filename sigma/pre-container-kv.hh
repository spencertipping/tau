#ifndef σpre_container_kv_h
#define σpre_container_kv_h

#include "../tau.hh"
#include "pre-container.hh"
#include "pre-kv.hh"
#include "pre-sstream.hh"

#include "../tau/begin.hh"

namespace σ::pre
{


struct kvsat_ : public virtual at_
{
  kvsat_(cback const &l, Sp<kv_> db) : at_(ct_set{}, l), db(db) {}

  void α(ηic &x, ηic&, ξo)   override { db->set(x, ηic{m, 1}); }
  void ω(ηic &x, ηic&, ξo)   override { db->del(x); }
  void ι(ηic &x, ηic&, ξo o) override { o.r(x.lsize() + 2) << x.all() << db->has(x); }
  void τ(ηic &x, ξo o)       override { db->sync(); o.r() << ηsig::τ; }

  Sp<kv_> db;

  static constexpr u8 m[] = {0x11, 0};  // int8 0
};


struct kvmat_ : public virtual at_
{
  kvmat_(cback const &l, Sp<kv_> db) : at_(ct_map{}, l), db(db) {}

  void α(ηic &k, ηic &v, ξo) override { db->set(key(k), v); }
  void ω(ηic &k, ηic&, ξo)   override { db->del(key(k)); }
  void ι(ηic &k, ηic&, ξo o) override
    { let r = db->get(key(k));
      if (!r.empty()) o.r(k.lsize() + r.lsize() + 8) << k.all() << r.all();
      else            o.r(k.lsize() + 2)             << k.all() << ηsig::ω; }
  void τ(ηic &x, ξo o) override { db->sync(); o.r() << ηsig::τ; }

  Sp<kv_> db;

  ηi key(ηic &k) { return k.is_η() ? k.η() : k; }
};


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
// We keep an in-memory stage so we aren't flushing things to the database too
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

struct kvmmat_ : public virtual at_
{
  typedef So<ηm, ηsstream_cmp> stage;

  typedef ηic key;  // user-facing key
  typedef ηic ind;  // key that we expect to be indirect

  kvmmat_(ct_multimap const &m, cback const &l, Sp<kv_> db)
    : at_(m, l), db_(db), ss_(0),
      svo_(1048576 * 64), sko_(65536), lvs_(8192),
      nk_((isha2{} << (ηm{} << now().time_since_epoch().count()))()),
      csw_([this] { flush(); }) {}

  ~kvmmat_() { flush(); }


  void α(key &k, ηic &v, ξo)   override;
  void ω(key &k, ηic &v, ξo)   override;
  void ι(key &k, ηic&,   ξo o) override;
  void κ(key &k, ηic&,   ξo)   override;
  void τ(ηic &x, ξo o)         override;


  // FIXME: add directly to literal if it is within size

  // FIXME: allow tuning

  void flush();
  void flush(key &k);

  ηsstream ss(key &k) const;  // unified view of values at key


protected:
  void flush_();        // assumes lock is acquired
  void flush_(key &k);  // assumes lock is acquired

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
  uN       kv_ilen     (key &k) const;  // #indirects, or 0 for literal
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
  ηm lkey(ηic &k)        const { return ηm{} << "l" << k.all(); }
  ηm ikey(ηic &k)        const { return ηm{} << "i" << k.all(); }
  ηm ikey(h256 const &k) const { return ηm{} << "i" << k; }


  Sp<kv_>      db_;
  mutable Smu  lock_;  // lock for all mutations
  M<ηm, stage> add_;   // staged values to add
  M<ηm, stage> del_;   // staged values to delete
  iN           ss_;    // staged size = ∑|v| in stage
  iN           svo_;   // staged value overflow (bytes)
  iN           sko_;   // staged key overflow (#keys)
  iN           lvs_;   // literal value size limit (bytes)
  h256         nk_;    // seed for genkey()
  Λcsw         csw_;   // auto-flush on context switch
};


}

#include "../tau/end.hh"

#endif
