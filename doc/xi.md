# ξ bounded channels
ξs are one-directional data pipes that cause [λs](lambda.md) to yield if they are blocked (either on sending or receiving). They carry any length-framed value, typically [η](eta.md). ξs don't inspect their values beyond asking about their "size overhead", which defaults to the literal size of the value but may be defined otherwise.

Each ξ, unless it is weakened, holds a reference to the [ψ](psi.md) that writes into it.

ξs present as allocators when writing; you can write and adjust the size of your value before committing it. This allows you to write without copying, for instance when doing file IO.


## ξ allocation
This happens in two stages. First, you indicate how much contiguous memory you intend to use; for example, let's say you request 256 bytes. The ξ replies with a `Sn<u8>` sized to the largest contiguous space ≤256 bytes that it can be. **This step may block.**

Once you're done writing the memory, you must do one of two things:

1. Call `commit()` to make the value available
2. Call `abort()` to erase the new value

`commit()` can accept an optional size; if you provide this, it will trim the value to that size, which saves memory.


## ξ states
An open ξ has two paths to destruction:

+ Reader closes → ξ is immediately destroyed
+ Writer closes → reader consumes data until EOF → ξ is destroyed automatically at EOF

Note that the writing ψ becomes eligible for GC as soon as _either_ ξ endpoint closes the channel; at that point there is no further connection between the ψs.


## Weak ξs
It is possible to create a ξ that doesn't GC-pin its source ψ. This is useful for diagnostics and logging, for example. If a ψ connected to a weak ξ is destroyed, it will appear as though that ψ has closed the ξ normally.


## ξ bounds and overflow
ξs fully serialize all values into bytestreams and are bounded by total capacity. ξs can carry overflowing values by allocating a one-off sidecar buffer, but only if they have no other data. (So a 64-byte ξ could carry a 256-byte η, but only if it were otherwise empty. It would temporarily allocate a larger buffer, then drop the buffer as soon as the reader had consumed the η.)
