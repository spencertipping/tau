# ξ bounded channels
ξs are one-directional data pipes that cause [λs](lambda.md) to yield if they are blocked (either on sending or receiving). They send and receive atomic [η](eta.md) values.

Each ξ, unless it is weakened, holds a reference to the [ψ](psi.md) that writes into it.


## ξ states
An open ξ has two paths to destruction:

+ Reader closes → ξ is immediately destroyed
+ Writer closes → reader consumes data until EOF → ξ is destroyed automatically at EOF

Note that the writing ψ becomes eligible for GC as soon as _either_ ξ endpoint closes the channel; at that point there is no further connection between the ψs.


## Weak ξs
It is possible to create a ξ that doesn't GC-pin its source ψ. This is useful for diagnostics and logging, for example. If a ψ connected to a weak ξ is destroyed, it will appear as though that ψ has closed the ξ normally.


## ξ bounds and overflow
ξs fully serialize all values into bytestreams and are bounded by total capacity. ξs can carry overflowing values by allocating a special sidecar buffer, but only if they have no other data. (So a 64-byte ξ could carry a 256-byte η, but only if it were otherwise empty. It would temporarily allocate a larger buffer, then drop the buffer as soon as the reader had consumed the η.)
