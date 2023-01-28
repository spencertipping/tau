# η₀ format
η₀ is the plumbing of [η](eta.md): it's possible to write a GC that knows only about η₀ structure and can rewrite a heap, including references. η₀ frames have a few different formats, all of which are equivalent up to the length of various subfields:

```
0tttttss                              data...  ← short frame
10tttttt fsssssss                     data...  ← medium frame
110fcbbb tttttttt s[(b+1)*8]          data...  ← long frame
111hcbbb tttttttt s[(b+1)*8] sha3[32] data...  ← disk frame
```

**NOTE:** disk frames may be separated by sentinel-encoded boundary frames, which overlap with η₀ frames but are cryptographically detectable given the file header. This encoding enables random access and is beyond the scope of the η₀ spec -- but see [η₀ seekable files](eta0-seekable.md) for details.

+ `f` indicates that the frame contains η₀ references
+ `c` indicates that the frame data is `zstd`-compressed
+ `h` indicates that the frame data is prepended with its SHA3-256
+ `b` is the number of bytes used to describe the size (`0` has special meaning)
+ `t` is the type ID
+ `s` is the size, stored big-endian

Short frames don't include `f` because `f = (t == 0)` and they can't contain subframes.

If a frame is compressed and hashed, `hash = sha3_256(zstd(data))`.

**NOTE:** short frames must not include padding; the size is assumed to be informative.


## Compressed frame data
Compressed data always begins with the size of the uncompressed data, which is encoded as a 64-bit big-endian unsigned int. So the frame would look like this:

```
11__1___ T S H? US zstd...
```

...where `US` is the uncompressed size and `zstd...` is the compressed datastream.

**NOTE:** `US` is included in the SHA3 if the data is compressed.


## Basic API
```cpp
enum class η0t : u8;
struct η0i             // input
{
  η0i(u8c*);           // pointer to frame

  uN   osize() const;  // outer size
  uN   hsize() const;  // header size

  uN   size() const;   // net size
  u8c* data() const;   // decoded data
  η0t  type() const;   // type code

  bool f() const;      // is flagged
  bool c() const;      // is compressed
  bool h() const;      // is hashed
  bool v() const;      // is the data valid against the hash?

  // TODO: do we need this?
  bool o() const;      // is this frame optimally packed?
};
```


## References and GC
`t == 0` is reserved for η₀ references, which are native-sized, native-endian pointers encoded as short frames. Any value that contains a pointer within its `data...` will have `f = 1`.

**NOTE:** η₀ references are not supported yet.
