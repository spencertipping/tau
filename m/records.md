# Frames and records
A frame is a single chunk of data for which atomic transit is guaranteed, and which works like this:

```
struct frame
{
  u8  magic[4] = "\xff\xffu9";
  u32 length;
  u64 schema[];
  u8  data[];  // length specified by schema
};
```

`schema` is a variable-length array of `u64` values that ends in a single schema-_τ_ terminator entry.

**TODO:** define `schema` format, probably 16x four-bit elements
