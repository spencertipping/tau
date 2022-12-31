# Γ
A composition of [γ](gamma.md) elements that form an eigenvector within a larger τ system.

Γ can have its own "control φ" that is used to signal that it should be destroyed. The control φ can also emit performance/diagnostic information and enable debugging.

Γ can be thought of as the control plane for γ pipelines.


## Examples
+ Asqi
  + Inbound SSH connection ↔ git process
  + Inbound HTTP connection ↔ user session
  + User navigation → document OT connection
  + User search/nav action → search prediction
  + Θ → square/stripe status → kubectl
  + Search indexer
  + Search query
+ Map/reduce
  + Input file → mapper → sort → combiner → shuffle input
  + Shuffle output → reducer → output
  + Disk-backed merge sort
+ `ni`/ETL
  + CLI → pipeline
  + Sharding → [pipeline]
  + Horizontal scaling → [pipeline]


## Encoding
A π₀ map with:

+ A set of γs
+ A set of boundary φs, each named
+ A multiplexing γ that connects a single control φ to each internal γ's control φ
  + This Γ controller also detects and handles individual γ destruction

Just as a γ's behavior is mainly templated by its topology and initial λs, Γ's behavior is templated by its control γ.
