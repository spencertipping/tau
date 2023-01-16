# η₀ format
η₀ is the plumbing of [η](eta.md): it's possible to write a GC that knows only about η₀ structure and can rewrite a heap, including references. η₀ frames have a few different formats, all of which are equivalent up to the length of various subfields:

```
0tttttss                 data...          ← short frame
10tttttt fsssssss        data...          ← medium frame
110fcbbb tttttttt s[b*8] data...          ← long frame
111hcbbb tttttttt s[b*8] hash... data...  ← disk frame
```

**TODO:** fix up `disk frame`; we should have boundary hashes for stream syncing (or at least leave room for it)

+ `f` indicates that the frame contains η₀ references
+ `c` indicates that the frame data is `zstd`-compressed
+ `h` indicates that the frame data is prepended with its SHA3-256
+ `b` is the number of bytes used to describe the size (`0` has special meaning)
+ `t` is the type ID
+ `s` is the size, stored big-endian

Short frames don't include `f` because `f = (t == 0)` and they can't contain subframes.

If a frame is compressed and hashed, `hash = sha3_256(zstd(data))`.

**NOTE:** short frames must not include padding; the size is assumed to be informative.


## Basic API
```cpp
struct η0i             // input
{
  η0i(u8c*);           // pointer to frame

  uN   osize() const;  // outer size
  uN   hsize() const;  // header size

  uN   size()  const;  // net size
  u8c* data()  const;  // decoded data
  uN   type()  const;  // type code

  bool is_e()  const;  // is size exact
  bool is_f()  const;  // is flagged
  bool is_c()  const;  // is compressed
  bool is_h()  const;  // is hashed

  bool is_o()  const;  // is this frame optimally packed?
  bool is_v()  const;  // is the data valid against the hash?
};
```

Values can be assembled with a streaming interface (for collections):

```cpp
struct η0s
{
  η0s(η0i, bool f);              // copy, maybe flatten, and optimize
  η0s(uN   type,
      bool c,
      bool h,
      uN   size = 0);

  η0s &add(u8c *data, uN size);  // plus overloads for other data
  η0s &end(u8c *data, uN size);  // plus overloads for other data
  η0s &end();                    // finalizes the stream

  uN osize() const;
  uN write(u8 *dest);
};
```

...or with a simple interface for single values:

```cpp
template<η0T T>
uN η0w(η0s&, T const&);  // return osize of value
```

**FIXME:** there should be a lot of parity between [ζ](zeta.md) and any η₀ writer API; we should be able to stream things into the heap and/or into channels


## References and GC
`t == 0` is reserved for η₀ references, which are native-sized, native-endian pointers encoded as short frames. Any value that contains a pointer within its `data...` will have `f = 1`.
