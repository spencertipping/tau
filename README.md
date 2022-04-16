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
