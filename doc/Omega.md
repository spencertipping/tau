# Ω persistence
[η](eta.md) is a container for primitives in memory. Ω is a container for η values in files. There are different variants for different situations. Starting with low-level structures:

+ `Ωf`: a single file containing a single η value, atomic + durable, eventually consistent
+ `Ωl`: an append-only compact log of framed η values, atomic + durable
+ `Ωm`: a file of mutable values that can be moved/deallocated (**TODO**)
+ `Ωh<K, V>`: a persistent, mutable hash index
  + `K → u64` is a uniformly-distributed keyspace
  + `V` is a fixed-size object at each key


## Ωf
A short header followed by a bare η in a file, updated with a full rewrite and atomic `rename`. The header is updated when a new file has been written, at which point interested readers can reopen the file to get the new value.

```
Ωf\0      # 4-byte magic
rev:u32b  # revision count, incremented on complete update (write + rename)
η...      # data
```


## Ωl
A header followed by many η values, each framed by its length. The header contains a `u64b` length field that describes the number of bytes in the file that are currently valid.

```
Ωl\0           # 4-byte magic
resv:u32b      # 4 bytes of reserved stuff, currently 0
size:u64b      # number of valid bytes (including header), updated after append
n₁:u32b η₁...  # n₁ bytes of η data
n₂:u32b η₂...  # n₂ bytes of η data
...
```


## Ωh
A file similar to [LSM](https://en.wikipedia.org/wiki/Log-structured_merge-tree) that stores sorted runs of hashed key data, mapping to arbitrary fixed-length values. Keys are hashes that may collide, so retrieval typically involves returning zero or more candidates for a given key insertion.

```
Ωh\0               # 4-byte magic
rev:u32b           # 4-byte revision, incremented by writer
hsize:u32b         # header size in bytes
n:u32b             # number of active arrays

a1o:u64b a1s:u64b  # first array offset+size
a2o:u64b a2s:u64b  # second array offset+size
...
aNo:u64b aNs:u64b  # final array offset+size
...                # padding until byte hsize

array1...          # byte hsize: array1 data begins
array2...
...
arrayN...
```

Some important points:

1. The ordering of arrays in the header need not correspond to the order in which they are stored
2. Arrays are compacted in a background process


### Locking
Ωh files are mutable, so advisory locks coordinate data access across processes.

**TODO:** specify exact strategy
