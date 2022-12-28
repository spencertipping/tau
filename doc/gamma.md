# γ
A [π₀](pi0.md)-hosted abstraction that contains λs and [φs](phi.md). The idea is to jointly define (1) polymorphic λ/φ containers, and (2) polysemantic connection operators.

τ Laplacian eigenvectors are just π₀ functions that achieve these φ connections, so γ operators are eigenvector constituents.


## Examples
+ Asqi
  + Inbound SSH connection ↔ git process
  + Inbound HTTP connection ↔ user session
  + User navigation → document OT connection
  + User search/nav action → search prediction
  + Θ → square/stripe status → kubectl
+ Map/reduce
  + Input file → mapper → sort → combiner → shuffle input
  + Shuffle output → reducer → output
+ `ni`/ETL
  + CLI → pipeline
  + Sharding → [pipeline]
  + Horizontal scaling → [pipeline]


## `φ*` vs `φ|`
Many operations like JSON encode/decode are 1:1 against φ elements. Those don't require separate output φs; we can fuse their transformations into map-loops to save φ IO. It's fine with me if we restrict this optimization to strict 1:1 operations; filters can drop through φ.

Because π₀ functions are concatenative, we can φ* onto either the left or right side of the λ.


## φ parsing
It would be useful to have a chunk-merged parser for stuff like websockets. Ideally it doesn't involve unrolling each byte into a separate UTF9, although φ allocation is so fast that it probably isn't the end of the world if we do.

**Q:** what does a stream parser look like? Probably just a state machine + an update function. The update function can unroll the input or not, maybe selecting input by a prefix.

If the input is combinable (e.g. string pieces), then maybe we can simplify by implementing `(state, chunk) → (state', n_consumed, [results...])` like a PEG. `state` should probably be a key in function table, or better yet an index into a tuple.

**Update:** we need `state` to contain a bit more than that; it also needs to hold data, e.g. websocket mask bytes, transfer encoding, or remaining content length. So our actual parse function would go like this:

```
(state, data, chunk, start) → (state', data', n_consumed, [results...])
```

`[results...]` are written to the output φ immediately.


### State encoding
Because we're in a stack environment, we encode the parse state like this:

```
data... state chunk start → data'... state' n_consumed [results...]
```

`data` is carried over as a stack residual, and can have any number of elements. φ parsing makes no assertion about whether it changes in size.
