# Ω persistence
Ω is terminal storage for [η](eta.md) key/value pairs, with an emphasis on write-once read-many usage. Ω uses two files per logical table:

+ `db.kv`: the log of keys and values, with length prefixing (can be navigated sequentially)
+ `db.hm`: a hash-mapped index against `kv`, with _O(lg lg n)_ average-case lookup time

Ω is a low-level storage layer and not a database; for example, it doesn't provide transactions or atomicity beyond the local process, in which all operations are fully serialized. Other processes don't have atomic views of data, although they may be able to infer it with a little bit of extra coordination.


## `kv`/`hm` split
`kv` is the append-only log that collects values; `hm` provides a fast mapping from `hash(k)` to file offsets within `kv`. Here are the basic formats:

+ `kv` = `klen:u16b vlen:u32b k[klen]... v[vlen]...`, where `klen=0` means the value has been deleted
+ `hm` = `hash:u64b offsize:u64b`, sorted by `hash`

`hm` files typically contain multiple runs of sorted data organized within a header; see sections below for details.


## `hm` multi-run format
We don't want to rewrite the whole index every time we add values to the database. Instead, we write smaller runs that are searched in parallel until they are fully merged -- this happens automatically as values are written, and will block writers for short periods of time. Longer merges can also happen, but in background threads.
