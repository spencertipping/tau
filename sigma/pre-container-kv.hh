#ifndef σpre_container_kv_h
#define σpre_container_kv_h

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

  void α(ηic &k, ηic &v, ξo) override { db->set(k, v); }
  void ω(ηic &k, ηic&, ξo)   override { db->del(k); }
  void ι(ηic &k, ηic&, ξo o) override
    { let r = db->get(k);
      if (!r.empty()) o.r(k.lsize() + r.size() + 8) << k.all() << r.all();
      else            o.r(k.lsize() + 2)            << k.all() << ηsig::ω; }
  void τ(ηic &x, ξo o) override { db->sync(); o.r() << ηsig::τ; }

  Sp<kv_> db;
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
// ("l" "foo") → 0 1 2 3 4      # literal key "foo" with values 1 2 3 4
// ("l" "bar") → 2 "k1" 2 "k2"  # "foo"'s values are stored at "k1" and "k2",
// ("i" "k1")  → 1 3            #   each of which has two values
// ("i" "k2")  → 2 4
//
// We keep an in-memory stage so we aren't flushing things to the database too
// eagerly. This allows us to write a batch of values into an indirect key and
// avoid fragmentation in the process.
//
// Insertion, deletion, and retrieval can all kick off a single-key merge, which
// causes all indirect values to be flattened into a single list. This is done
// with η sorted streams, so it's fast and space-efficient.

struct kvmmat_ : public virtual at_
{
  kvmmat_(ct_multimap const &m, cback const &l, Sp<kv_> db)
    : at_(m, l), db_(db), ss_(0),
      svo_(1048576), sko_(64) {}

  void α(ηic &k, ηic &v, ξo)   override;
  void ω(ηic &k, ηic &v, ξo)   override;
  void ι(ηic &k, ηic&,   ξo o) override;
  void τ(ηic &x, ξo o)         override;

  void stage(ηic &k, ηic &v);    // add to stage
  void merge(ηic &k, uN n = 1);  // merge all but n indirects into a single key
  ηm   flush(ηic &k);            // flush stage to indirect key, return key
  void flush();                  // flush all staged values

  ηsstream at(ηic &k);           // stream of values at key

  Sp<kv_>                     db_;
  M<ηm, So<ηm, ηsstream_cmp>> add_;  // staged values to add
  M<ηm, So<ηm, ηsstream_cmp>> del_;  // staged values to delete
  uN                          ss_;   // staged size = ∑|v| in stage
  uN                          svo_;  // staged value overflow (bytes)
  uN                          sko_;  // staged key overflow (#keys)
};


}

#include "../tau/end.hh"

#endif
