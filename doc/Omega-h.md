# Ωh
A file similar to [LSM](https://en.wikipedia.org/wiki/Log-structured_merge-tree) that stores sorted runs of hashed key data, mapping to arbitrary fixed-length values. Keys are hashes that may collide, so retrieval typically involves returning zero or more candidates for a given key insertion.

```
Ωh\0               # 4-byte magic
rev:u32b           # 4-byte revision, incremented by writer
capacity:u32b      # maximum number of active arrays
n:u32b             # number of active arrays

a1o:u64b a1s:u64b  # first array offset+size at byte 16
a2o:u64b a2s:u64b  # second array offset+size
...
aNo:u64b aNs:u64b  # final array offset+size
...                # padding until byte hsize (arbitrary data)

array1...          # byte 16 + 16 * capacity: array1 data begins
array2...
...
arrayN...
```

Some important points:

1. The ordering of arrays in the header need not correspond to the order in which they are stored in the file (i.e. `array2` can be located before `array1`)
2. Arrays are compacted in a background process
3. Readers must release locks quickly; they are not allowed to hold array locks for longer than absolutely necessary


## Staging and compaction
Insertions are staged in an `unordered_multimap` and are written into new arrays, which are merged synchronously by the writer. Merging applies to the smallest _k_ of _n_ arrays, and happens automatically when:

1. We write a new array whose size is ≥50% of the smallest array
2. We have more than _f · log₂(largest array)_ arrays, where _f_ is tunable
3. The user requests compaction


## Locking
Ωh files are mutable and use advisory locks for inter-process coordination. The lock regions are:

+ `rev`: locked exclusively by the writer
+ `a1o..aNs`
  + Writer locks exclusively during header updates
  + Readers lock shared during lookups
+ **TODO:** `n` (if we need traffic control)
  + Readers lock and then instantly unlock
  + Writer locks and holds until arrays are unlocked

The `hrev` lock verifies that only one writer exists at any given time and serves no other purpose.

The `a1o..aNs` lock allows readers to claim existing arrays, preventing them from being overwritten. The exclusive lock is required to delete or reuse any existing array space, although it is _not_ required to append new arrays or use already-free space. So the writer follows the pseudocode logic below.

`n` prevents writer starvation if many readers are consistently active. I don't think it's necessary yet, but if it becomes so then I'll add it.

```
void allocate(array xs)
{
  if (we_are_destroying_stuff())
  {
    lock_arrays();    // we now hold the exclusive lock
    // overwrite/destroy any arrays we'd like to
    unlock_arrays();  // new arrays are ready to use
  }
  else
  {
    write_into_free_space(xs);  // reuse free or append
    lock_arrays();              // just a metadata update
    update_header();            // drop consumed arrays, add new one
    unlock_arrays();            // new arrays are ready to use
  }
}
```

Readers are simpler; they just need to lock the arrays long enough to find things:

```
int find(K x, V *vs, int n)
{
  shared_lock_arrays();     // no exclusive locks can exist beyond this point
  do_find_stuff(x, vs, n);
  shared_unlock_arrays();   // we're done, arrays can be updated
}
```
