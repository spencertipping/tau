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
  + AWS/Azure billing
  + AWS/Azure DNS
+ Network
  + TCP server: `β → (φ, conninfo) → λ → δ`
  + SSH server: `β → (φ, conninfo) → λ → δ`
  + HTTP/WS server: `β → (φ, url, method, headers) → λ → δ`
  + TCP client
  + SSH client
  + WS client
+ File IO
  + Sequential file reader (bytes)
  + Sequential file writer (bytes)
  + Stream compressors/decompressors
  + Bytes → UTF9 frame assembly
  + UTF9 → bytes
  + Random file reader/writer (bytes)
  + Bytes → lines
  + SQLite as K/V store
+ Computation
  + All of these are inline-modifiable with `φ*<` and `φ*>`
  + [π₀ 1:1 mapper](gamma-phi-map.md)
  + [π₀ many:many mapper](gamma-phi-map.md)
  + [π₀ streaming parser](gamma-phi-parse.md)
  + In-memory cyclic sort by π₀ expression
+ Distributed/topology
  + Data-independent scaling, runtime-modifiable
  + Data-dependent sharding, runtime-modifiable
  + Merge cyclic-sorted streams, runtime-modifiable
  + Merge unsorted streams, runtime-modifiable
  + _β_-replication (blocking)
  + _β_-replication (non-blocking)
  + Size-batching into cycles
  + `φ(τ) → φ(φ)` striping (via file buffers, probably)
  + Forked-process `(φ, pid_fd)`
  + Process-waiting φ
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


## Lifecycle
γ objects go through three stages:

1. Configuration: created but not fully hooked up
2. Runtime: hooked up and running
3. Destruction: manages unhooking and λ/φ destruction

Configuration is when `φ*<` and `φ*>` can be applied to boundary φs. No external changes can be made to a γ during runtime; once _δ_ φ is _ω_, it destroys its λs and disconnects/destroys its φs.

Any of a γ's λs can _ω_ its _δ_ φ. If one does, then the containing Γ control will also observe a _ω_ and should react accordingly. **This does not necessarily mean all γs should be terminated.**

For example, a linear data pipeline should terminate left-to-right: if `b` exits early within `a | b | c`, `a` can be pre-terminated but `c` must be allowed to run. Note that this is more precise than bash/UNIX process management, where `a` waits for `SIGPIPE` before exiting.

Destruction policy is part of Γ polymorphism.


## Encoding
A π₀ map with:

+ A set of λ pre/post filters (for `φ*`)
+ A set of boundary φs, each named
+ Once running, a set of λis
+ A finalizer function, or the default `[]`
+ A "control φ" that, when closed, will cause the γ to be destroyed

Note that γ destruction is a generic process that involves the finalizer function, `λ_`, and `φ_` in that order.

Each λ receives a mutable reference to the full γ map as its init argument.
