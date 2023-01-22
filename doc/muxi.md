# μ/ξ bounded channels
Channels are one-directional data pipes that cause [λs](lambda.md) to yield if they are blocked (either on sending or receiving). They send and receive atomic [η](eta.md) values.


## Channel states
An open channel has two paths to destruction:

+ Reader closes → channel is immediately destroyed
+ Writer closes → reader consumes data until EOF → channel is destroyed automatically at EOF

Note that the writing ψ becomes eligible for GC as soon as _either_ channel endpoint closes the channel; at that point there is no further connection between the ψs.


## Weak channels
It is possible to create a channel that doesn't GC-pin its source ψ. This is useful for diagnostics and logging, for example. If a ψ connected to a weak channel is destroyed, it will appear as though that ψ has closed the channel normally.


## μ channels
μs carry pointers to π shared heap values, and are bounded by the number of values they can refer to. Internally, μs are π GC roots.


## ξ channels
ξs fully serialize all values into bytestreams and are bounded by total capacity. ξs can carry overflowing values by allocating a special sidecar buffer, but only if they have no other data. (So a 64-byte ξ could carry a 256-byte η, but only if it were otherwise empty. It would temporarily allocate a larger buffer, then drop the buffer as soon as the reader had consumed the η.)
