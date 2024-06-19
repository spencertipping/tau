# Ω persistence
[η](eta.md) is a container for primitives in memory. Ω is a container for η values in files. There are different variants for different situations.


## Low-level structures
+ [`Ωf`: atomic η value](Omega-f.md)
+ [`Ωl`: append log of η values](Omega-l.md)
+ [`Ωh<K, V>`: a persistent, mutable hash index](Omega-h.md)
  + `K → u64` is a uniformly-distributed keyspace
  + `V` is a fixed-size object at each key


## High-level structures
+ `Ωa = Ωl + Ωh`: an insert-only key/value store
  + `.add(k, v)`
  + `.get(k) → v | empty`
  + `.commit(fsync)`
+ [`Ωs = Ωh`: search index](Omega-s.md)
  + `.add(k, v)`
  + `.get(k) → vs...`
  + `.commit(fsync)`
  + `.repack()`
