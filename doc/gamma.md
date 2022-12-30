# γ
A [π₀](pi0.md)-hosted abstraction that contains λs and [φs](phi.md). The idea is to jointly define (1) polymorphic λ/φ containers, and (2) polysemantic connection operators. τ Laplacian eigenvectors are just π₀ functions that achieve these φ connections, so γ operators are eigenvector constituents.


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


## Data transformation
+ [`φ*` and `φ|`](gamma-phi-map.md)
+ [`φ=`](gamma-phi-parse.md)


## γ state
We need to represent a few things:

1. Individual components
2. Edits to those components
3. Pipelines of multiple components
4. Connection slots for pipelines

(4) isn't explicit; it's a property of components that are eigenreactive -- and therefore create new γs that slot into those eigenvector positions.

(1) and (2) form the core component API, which creates and configures γs that are stored on the stack. (3) allows polysemantic connections between these components; it deals with "which φs connect to which other φs".

**TODO:** how do we manage φ destruction in a pipeline context? Seems like we need an implicit control channel.
