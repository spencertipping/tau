# Ω `h→m` mapping
First things first: how do we pack a hash, offset, and size into just 16 bytes, especially if the hash is strictly 8 bytes? It involves using just 64 bits for the offset + size, which we can do pretty easily. Let's set an upper data-file size to 16TiB, so we have an offset size of 44 bits. The remaining 19 bits lossily encode the size, which can be up to 4GiB per record:

```
exp:5      trim:15                         offset:44
|---| |----------------| |-----------------------------------------------|
eeeee ttt ttttttttt tttt oooo oooooooo oooooooo oooooooo oooooooo oooooooo
```

`offset` is literal; `size` is calculated similar to floating-point:

```
size = (1 << uint5(exp) + 1) * (1.0 - uint15(trim) / 65536.0)
```

This gives us exact packing with overreads limited to ~0.003% and lossless encoding for all objects smaller than 64kiB.


## Size-inexact retrievals
[η](eta.md) does not encode the length of the value being stored; it relies on you to have that information, and it needs to be exact to avoid decoding errors. That means that although `h→m` mappings can approximate the size for retrieval purposes, we must eventually make up the error.

There are a couple of approaches we can take:

1. Save space and prepend a `u16b` residue for any large values, no residue for small values
2. Prepend a `u32b` onto every value so we get full data-log replay for recovery

I prefer the second option: we do lose some space, but debugging is a lot easier and we have a full replay log in case the `h→m` files become corrupted. See [Ω `k→v` log files](Omega-kv.md) for details.


## Special `m` values
**TODO:** we can get more mileage out of subnormal `e` values, which currently have redundant encoding. This would make room for special flag values.
