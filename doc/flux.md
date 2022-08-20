# Flux
Ways to move values around. Flux conduits (ζ) are unidirectional queues for values that can be serialized inline to byte buffers. The serialization API looks like this:

**TODO:** condense this doc, refactor interfaces to "reader" and "writer" frontends

```cpp
template<class T, class R = u64>
struct ζ
{
  u16  id;    // how to refer back to this ζ
  Λ   &l;     // for fiber control
  ...
  ζ &c(uN);   // reserve capacity
  ζ &m(uN);   // mark completed object extending to specified offset

  // write a full object at the current insertion point, then mark its
  // upper size as the new insertion point
  R operator<<(T const&);

  // primitive serialization functions
  ζ &U8 (uN, u8);
  ζ &U16(uN, u16);
  ζ &U32(uN, u32);
  ζ &U64(uN, u64);

  ζ &I8 (uN, i8);
  ζ &I16(uN, i16);
  ζ &I32(uN, i32);
  ζ &I64(uN, i64);

  ζ &F32(uN, f32);
  ζ &F64(uN, f64);

  ζ &a(uN, u8 const *, uN);
  ζ &a(uN, Bs<u8>);
  ζ &a(uN, Bsv<u8>);
};
```

Deserialization has a similar API.

```cpp
template<class T, class R = u64>
struct ζ
{
  u16  id;
  Λ   &l;
  ...

  struct const_iterator {...};
  const_iterator begin() const;
  const_iterator end()   const;

  T operator[](R);

  // primitive deserialization functions
  u8 U8(uN);
  u16 U16(uN);
  ...
  f32 F32(uN);
  f64 F64(uN);

  ζ &a(uN, u8 * d,   uN n);  // read n bytes into d
  ζ &a(uN, Bs<u8>&,  uN n);
  ζ &a(uN, Bsv<u8>&, uN n);
};
```


## References
I want `sizeof(R)` and `sizeof(const_iterator)` to both be `sizeof(void*)`, which we can do in two ways:

1. Point to the byte buffer directly (bad idea)
2. Pack some bits to encode a relative offset

(2) is preferable because it lets ζ move the byte buffer, which is sometimes required for contiguous object access. If we do this, we need to encode a few things:

1. Which ζ contains the data
2. What `T` is (for safety)
3. The offset (within the ζ) of the object being referred to


## GC
ζs don't have GC proper, but they do collect memory as you free/destroy `R`s that are no longer in use. This happens by moving the "from pointer" forwards, decreasing the live-memory window.

It would be prohibitive to manage memory at the byte-level, so we manage pages instead. Each page (4KB) has a reference count that `R`s contribute to. Free pages are available for writing; pages with references are write-locked.
