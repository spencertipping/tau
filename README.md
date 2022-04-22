# Tau
Cyclic dataflow processing.


## Constraints
+ Allow for length-_suffixed_ frames to minimize transfer latency
+ Soft references
  + Destructure on a per-message basis and interpolate: `ot$A` to select an `ot` component by field `A` (implicit content-driven multiplexing)
  + How is GC done? Probably by the multiplexing manager
+ _τ_ closures
  + `τ[...]`
  + `τA` to create dynamic equivalence groups
  + **NOTE:** τ is forward-only

**NOTE:** **data structures are multiplexers** -- or more precisely, container structures use multiplexers to address specific values.


## v2 features
+ 2D/3D design environment with keystroke lensed cursors
  + Grammar is combinatory lenses
  + Keystrokes modify lens context (and therefore provide serialization)
  + State can be reconstructed as key sequences (for persistence)
  + Does an OT exist?
+ Structural placement depends on _topology,_ not hard-coded positions
  + Positional constraints are derived from topology + viewstate


## Representation
**TODO:** we probably don't need this for v1, as we can just do serial pipelines

+ Source should be thinly convertible to data
+ It should be easy to refer to arbitrary elements
