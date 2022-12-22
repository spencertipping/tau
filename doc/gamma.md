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
