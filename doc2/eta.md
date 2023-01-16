# η heap/container encoding
η data has a few properties:

1. It's contiguous, up to η₀ nonlinear references
2. A tag bit indicates whether it or any descendant contains a [η₀](eta0.md) reference
3. Its size is knowable in constant time
4. Its type is encoded as an integer, where `0` is reserved for η₀
5. Data is stored native-endian, sizes are stored fixed-endian

See [η₀ spec](eta0.md) for the bit-level format.


## Defined types
| Code | Description |
|------|-------------|
| 1    |             |
