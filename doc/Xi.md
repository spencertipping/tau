# _Ξ = f(ξ)_
A Ξ is a smart cable of [ξs](xi.md) that are awaiting connections. A Ξ has several aspects:

+ Shape (zero or more)
  + `f`: the Ξ has a primary forwards output
  + `b`: the Ξ has a primary backwards input
+ Laziness (exactly one)
  + `e`: the Ξ will immediately apply Γs to its ξs
  + `l`: the Ξ will collect Γs to be applied later on
+ Sidecars (zero or more)
  + `m`: the Ξ has a sidecar of η-keyed Ξs, e.g. from debug monitors
  + `s`: the Ξ has a sidecar of anonymous Ξs
  + `v`: the Ξ has named variables (`St → Ξ`)

**NOTE:** variables and sidecar values are always Ξs (often simple Ξs) because they may be full-duplex. Variables in particular might be arbitrarily complex, even though normally you wouldn't use them this way.

**TODO:** spec out the API
