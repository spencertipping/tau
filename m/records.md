# Frames and records
A frame is a single chunk of data for which atomic transit is guaranteed, and which works like this:

```
struct frame
{
  u8       magic[4] = "\xff\xffu9";
  u32      length;
  msgpack  schema;
  u8      *data;
};
```
