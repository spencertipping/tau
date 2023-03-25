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


## Using Ξs
There are a few common operations against Ξs:

+ Primary
  + Create Ξ from input and/or output
  + Transform primary with Ψ
  + Transform primary + variable with Ψ (var as monitor/control)
  + Bind a variable
  + Delete a variable
+ Sidecars
  + Transform sidecar with Γ
  + Multiplex keyed sidecars into a single channel
  + Combine a subset of sidecars into a multiplexed channel
  + Permute sidecar values in some way (re-key them, etc)

We also have stuff like "split Ξ into primary and tap", which is probably a Γ that binds a variable to capture the tap (or pushes it onto the anonymous stack).

I should probably outline use cases for each of these sidecar types.

+ `monitor`/`control`: `m` or `v`
+ `tap`/`join`: `s` or `v`

...what if `s` is just a stack and the primary connection is the top? Then fetching a variable is putting a new thing on top of the stack. That makes a lot of sense.
