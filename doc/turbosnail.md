# TurboSnail (TM) database
**Why?** Because LMDB is awkward and unreliable, although I like it otherwise. TurboSnail is designed to be as close to a synchronous `std::map` as possible, with the exception of transaction boundaries that allow you to specify how much data you want to write at once. For example:

``` cpp
turbosnail db{"foo.db"};  // create/open

{
  auto w = db.writer();   // start a write transaction (locally serialized)
  w.set("foo", "bar");    // immediately visible to same process
  w.del("bif");           // same here
  // w is committed here (saves to disk)
  // w.commit(true);      // could do this to fsync()
}
```


## Disk format
Each database is a directory of files, which contains:

```
index  ← small; contains a single η value
ist1   ← immutable sorted index for idt1
idt1   ← immutable data table
ist2   ← immutable sorted index for idt2
idt2   ← immutable data table
...
```

`index` contains a single [η](eta.md) value describing the database head state. See below section for details.

Each `ist` file is just a list of `u64b k, v` pairs, each of which describes the final bytes of key/value, respectively, within the corresponding `idt`. `ist` files are intended to be memory-mapped.

`idt` files are bare data whose offsets are provided by `ist`. For example, suppose `ist` contains these bytes in hex:

```
00000000 00000004  00000000 0000000e  # 4-byte key, 14-byte value
00000000 00000007  00000000 00000014  # 3-byte key, 6-byte value
```

`idt` might contain these bytes:

```
0x33 'f' 'o' 'o'                      # bytes 0-3: "foo"
0x3d 0x01 "hello world!!\n"           # bytes 4-18
```

**TODO:** think about data locality; we should pack keys together and values together to reduce TLB churn

**TODO:** pack hashed keys instead of having var-length keys
