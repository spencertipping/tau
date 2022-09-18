# ξ for offline data processing
Terminal entries are file IO, boundaries are multiplexing, sharding, and network IO, and we need distributed process control (it should be fine to assume a stable ABI).

We should also define sorting primitives and τ boundary generators.


## File IO
We can trivially stream UTF9 entries into files and reassemble them, although framing makes it possible to split the file without sequentially scanning it. I'm not sure how much that matters just yet.

Compression can operate on a streaming or block basis. We should probably provide both. Same story for encryption.


### Files as queries
We can read the whole thing serially, or we can respond to requests for data at specific locations.


### Columnar formats
Probably worth doing, as these have consistently better performance than flat data. Can we infer a schema automatically so the user is unaware of the encoding?


## Network IO
Same as file IO: we have exactly the same packet/read reassembly process.


## Multiplexing/sharding
...are the same, I think. We'll need unions to tap data together from multiple sources; then we can implement a full shuffle. The topology looks like this:

```
          +-------+      +-------+
map₁ ---> |       |      |       | ---> comb₁
          | union | ---> | demux |
map₂ ---> |       |      |       | ---> comb₂
          |       |      |       |
...       .........      .........      ...
          +-------+      +-------+
```


## Process control
As discussed in [Γ](../Gamma.md), we can create [Φ](../Phi.md)s connected via encrypted trunks to extend the [Ξ](../Xi.md) fabric. This principle can apply locally or remotely; in either case the trunk can be modeled as a bidirectional socket.


## Sorting
