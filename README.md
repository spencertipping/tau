# Tau


## Constraints
+ Allow for length-_suffixed_ frames to minimize transfer latency
+ 2D/3D design environment with keystroke lensed cursors
  + Grammar is combinatory lenses
  + Keystrokes modify lens context (and therefore provide serialization)
  + State can be reconstructed as key sequences (for persistence)
  + Does an OT exist?
+ Structural placement depends on _topology,_ not hard-coded positions
  + Positional constraints are derived from topology + viewstate
+ Soft references
  + Destructure on a per-message basis and interpolate: `ot$A` to select an `ot` component by field `A` (implicit content-driven multiplexing)
+ _τ_ closures
  + `τ[...]`
  + `τA` to create dynamic equivalence groups


## Representation
+ Source should be thinly convertible to data
+ It should be easy to refer to arbitrary elements
