# Ω index file
Each Ω table is made of a [log file](Omega-log.md) and an index that provides fast lookups within that log. Values are added to both simultaneously, which is viewed coherently from the writing process and incoherently from other processes.

The index file is responsible for storing location+size information of records within the log. We do this efficiently using sorted arrays of hashed keys:

```
{hash1, hash2} = xxh3_128(k)

       kv pair 1               kv pair 2
|---------------------| |---------------------|
hash1:u64b offsize:u64b hash1:u64b offsize:u64b ...
```

`offsize` is a packed number that provides an exact offset and approximate size with following bit layout and semantics:

```
shift:4 psize:16 offset:44

offset = offset  // encoded verbatim, range = 16TiB
size   = shift == 0 ? psize : (1 << 16) + psize << shift
```

`size` is always at least as large as the value's size; this way we reserve sufficient space when reading from the log.


## Staging and repacking
`Ωi` keeps an in-memory cache of pending insertions and deletions. `commit()`, which is sometimes called automatically if the stage is full, appends a new sorted array to the index, increasing `k()` by one. You can call `repack()` to decrease `k()`, improving lookup efficiency and reducing overall size.

Fragmentation is a big performance issue, so we avoid it in two ways:

1. Arrays are always arranged by decreasing size
2. By extension, `commit()` auto-merges any arrays smaller than the data being committed

We also have two heuristics that reduce the number of arrays per index:

1. We auto-repack if there are more than `log₂(size)` arrays
2. We auto-repack any arrays smaller than 16MiB on `commit()`

I went with 16MiB as the default lower limit because it's large enough to consistently get good IO parallelism when addressed with `pread` and `pwrite`, but small enough that we can do the IO in less than 5ms on average. You can tune this quantity.


## Index durability
`Ωl` is durable by default if we assume that appends are safe, which is generally the case. `Ωi`, however, is not an append-only data structure if we consider repacking. That means we have the potential to lose data in some cases.

We can work around this by maintaining a table of active arrays in the index file header. It has a fixed size and looks like this:

```
"Ωidx\0" v:u16b  // header magic + version (8 bytes)
key:u64b         // header key: unique per writer
rev:u64b         // header revision: incremented per update
n:u64b           // number of active arrays (max = 254)

a1o:u64b a1s:u64b  // first array offset+size (always at byte 32)
a2o:u64b a2s:u64b  // second array offset+size
...
aNo:u64b aNs:u64b  // final array offset+size
...                // padding bytes until header size == 4096

array1...          // byte 4096: array1 data begins
array2...
```

The header allows us to have holes in the index file, which is important because we can append a compacted array and delete its source fragments atomically with a single header update. Then we can either punch holes in the file or reuse earlier space.

For now, `v = 1`. All other values are illegal.


## Coordination and write-lock claims
`rev` increases every time the header is updated. Readers watch the value and reload if it has been changed. If a change does happen, it means the entire index has potentially been invalidated.

`key` coordinates writers in a similar way: there can be only one writer at a time, and the writer updates `key` to an arbitrary value on startup. If at any point `key` is modified, all write operations cease and the writer-context is invalidated. This essentially signals that someone else has claimed the write lock out from under us, and we must be willing to lose the currently-staged data. We must no longer modify the index file in any way.

This locking/coordination strategy is not airtight. For example, a writer may invalidate the header twice, causing a reader to receive corrupt data if it is insufficiently attentive. Similarly, the write-claim case may be problematic if the claim is made in the middle of a slow `write()` call from the original writer. We don't necessarily know when all `write` operations have completed from the initial process.
