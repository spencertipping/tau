# Ω datastore
[η](eta.md) manages data in memory, Ω provides a persistent log with fast k/v retrieval. Ω has two pieces:

+ **Data log:** a sequential file of framed η values
+ **Index:** a reduction of the data log that provides _O(lg lg n)_ lookups

Because Ω is log-oriented, it's optimized for insertions rather than updates; if you update data frequently, you'll have to either repack the log periodically or explicitly specify that you want to update values in-place (and manage your own allocation).


## Format specifications
+ [Log file](Omega-log.md): `data.kv`
+ [Index file](Omega-index.md): `data.idx`


## Core API
Ω represents a log + index, although each piece is also available on its own. The Ω wrapper encases the key in a η container and writes that + value into a log record, hashing by the key. It also unifies multiple candidate values and chooses the appropriate one, usually the most-recently written. Finally, it marks deletions.

```cpp
struct Ω
{
  void add(ηic&, ηic&);  // add key/value pair
  void del(ηic&);        // remove key
  ηm   get(ηic&) const;  // value at key, or empty
  void commit(bool);     // write to files, optionally fsync
  Ωl  &log();
  Ωi  &index();
};

struct Ωl  // log file
{
  u64  append(ηic&);               // add a formatted entry
  void del(u64);                   // delete entry at location
  ηm   read(u64) const;            // read a formatted entry

  u64  append(u8c*, u32);          // add a new entry
  u32  read(u64, u8*, u32) const;  // read an existing entry
  u64  offset(u64)         const;  // entry data offset (file byte location)
  u32  size(u64)           const;  // exact entry size (capacity)
  u64  next(u64)           const;  // next entry
  fd_t fd()                const;  // underlying file
  void repack();                   // rewrite the file (expensive)
};

struct Ωi  // index
{
  typedef XXH128_hash_t ΩH;

  typedef ΩH          k;  // key being queried
  typedef P<u64, u32> v;  // "value", i.e. offset+~size in logfile

  void   add(k, u64 vo, u32 vs);
  void   del(k);          // remove association for key
  V<v>   get(k) const;    // fetch all candidate values for key
  fd_t   fd()   const;    // underlying file
  u64    size() const;    // size on disk
  u64    k()    const;    // number of fragments
  V<u64> ks()   const;    // fragment sizes, in bytes
  void   repack(u64);     // repack up to #bytes of data
  void   commit();        // write stage to file
};
```
