# Ω internals
Ω presents a fairly simple interface, but there are a few moving parts:

+ Database
+ Multirun file
+ Sorted run section
+ Compaction job
+ Memory stage


## Database
Provides toplevel hashing and interfaces with multirun files. Also manages background compaction jobs and holds the memory stage.

```cpp
struct Ω
{
  struct Ωh;  // element hash type

  ηm   get(ηic&);
  void set(ηic&, ηic&);
  void set(ηic&, ηm&&);
  void del(ηic&);

  void commit(bool fsync = false);
  void repack();
};
```


## Multirun file
A container for sorted runs, and the abstraction that manages the header metadata and sparseness.

```cpp
struct Ω::multirun
{
  u64           size()    const;
  Ω::sorted_run at  (u64) const;
};
```


## Sorted run section
```cpp
struct Ω::sorted_run
{
  struct Ωm;  // kv metadata

  u64  size       ()     const;
  bool may_contain(Ωhc&) const;  // bloom check
  Ωm   at         (Ωhc&) const;
};
```
