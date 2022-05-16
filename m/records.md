# Frames and records
A frame is a single chunk of data for which atomic transit is guaranteed: that is, each pipeline element is able to load the whole frame into memory.

When designing the frame format, it's worth considering the throughput of various pipelines to make sure we don't introduce excessive overhead. Since frames may be streamed into compressors, we should model throughput in the 100MB/s range. In that world, 1 byte = 10ns.

```
struct frame
{
  u32 magic = "\xff\xffu9";  // 40ns
  u32 length;                // 40ns
  u16 nfields;               // 20ns
  u4  schema[];              // nfields · 5ns -- padded to 8 bits
  u8  data[];                // 8-bit aligned
};
```

So we have a fixed overhead of 100ns/frame/core, which gives us a maximum throughput of 10M frames/second/core. That's workable.


## Schema
**TODO:** are we optimizing for small fixed-width fields, or larger ones? If small, how do we specify absolute offsets?
