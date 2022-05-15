# Frames and records
A frame is a single chunk of data for which atomic transit is guaranteed, and which works like this:

```
struct frame
{
  byte  magic[8] = "\xff\xffutf9\x00\x00";
  int   tag;
  int   n_fields;
  field fields[n_fields];
  byte  data[field_ends[n_fields - 1]];
};


struct field
{
  ... length;
  ... type;
};
```

I'm not sure how big `field` is yet, but it's probably 4 bytes long. Small is good for these things.

`tag` identifies the frame at a high level, for instance whether the frame represents data (positive) or a _τ_ marker (negative). `tag` provides context about how the frame is keyed, which is useful for ordering.

**Q:** should `tag` carry _τ_ information, or should `n_fields` carry it?
