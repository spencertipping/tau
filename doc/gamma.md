# γ
A [π₀](pi0.md)-hosted abstraction that contains λs and [φs](phi.md). The idea is to jointly define (1) polymorphic λ/φ containers, and (2) polysemantic connection operators. τ Laplacian eigenvectors are just π₀ functions that achieve these φ connections, so γ operators are eigenvector constituents.

γs are typically created within the context of a [Γ](Gamma.md), which manages multiple γs at once and automates their destruction and control-channel multiplexing.


## Examples
+ Asqi
  + Single-document OT merge
  + User state ↔ OT state sync pipe (sync may be generalized)
  + `libgit2` query
  + Session state
  + Square state
  + Kubectl state
  + InfluxDB metrics upload
  + Metrics profiler
  + Email service endpoint
  + Search indexer (probably a Γ in practice)
  + Search query (probably a Γ in practice)
+ Network
  + SSH server
  + SSH client
  + HTTP/WS server
  + WS client
  + Connection-level multiplexer
+ File IO
  + Sequential file reader (bytes)
  + Sequential file writer (bytes)
  + Stream compressors/decompressors
  + Bytes → UTF9 frame assembly
  + UTF9 → bytes
  + Random file reader/writer (bytes)
  + Bytes → lines
+ Computation
  + All of these are inline-modifiable with `φ*<` and `φ*>`
  + [π₀ 1:1 mapper](gamma-phi-map.md)
  + [π₀ many:many mapper](gamma-phi-map.md)
  + [π₀ streaming parser](gamma-phi-parse.md)
+ Distributed
  + Data-independent scaling (like `ni S`)
  + Data-dependent sharding (like `ni S\>`)
  + Sort by π₀ expression
  + Merge sorted streams
+ Θ
  + `Θr`: periodic _τ_
  + `Θp`: one-time _τ_ at specific deadline
  + `Θd`: delay each element
  + `Θ?`: measure flow rate (elements/second)
  + `Θ<`: bounded time-reordering queue
+ Flow control
  + Bounded LRU-forgetful queue (drop if consumer is too slow)
  + Bounded fractal-forgetful queue
  + Gated queue (use with `Θ?` to rate-limit)
  + Repeat register (emit most-recent value when pinged)


## Encoding
A π₀ map with:

+ A set of λ pre/post filters (for `φ*`)
+ A set of boundary φs, each named
+ Once running, a set of λis
+ A finalizer function, or the default `[]`
+ A "control φ" that, when closed, will cause the γ to be destroyed

Note that γ destruction is a generic process that involves the finalizer function, `λ_`, and `φ_` in that order.

Each λ receives a mutable reference to the full γ map as its init argument.
