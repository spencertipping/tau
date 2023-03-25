# _Ξ = f(ξ)_
A Ξ is a smart cable of [ξs](xi.md) that are awaiting connections. A Ξ has several aspects:

+ Shape
  + `f`: the Ξ has a primary forwards output
  + `b`: the Ξ has a primary backwards input
+ Laziness
  + `e`: the Ξ will immediately apply Γs to its ξs
  + `l`: the Ξ will collect Γs to be applied later on
+ Sidecars
  + `m`: the Ξ has a sidecar of η-keyed ξs, e.g. from debug monitors
  + `s`: the Ξ has a sidecar of anonymous ξs
  + `v`: the Ξ has named variables (`St → ξ`)
