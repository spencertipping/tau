# Frames and records
Using [msgpack](https://msgpack.org) for data transit. It's possible we'll have inlined custom extension records (which `msgpack` can encode with custom tags) that are negotiated on the fly for consistent schemas.

...so tau is a way to move `msgpack` objects around, plus some wrapping to give us tagged data structures. If we need more performance or space efficiency, we can offload to a binary codec stored in a `msgpack` extension field.

**NOTE:** we still need to frame the length onto the front of our `msgpack` objects, since `msgpack` objects don't have that framing.
