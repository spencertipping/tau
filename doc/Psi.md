# _Ψ ∷ Ξ → ([ψ], Ξ)_
Ψs are fixed-topology processors that can be wrapped into [Γs](Gamma.md) with adapters. There are several variants, each with optional aux connectors specifiable with a mask:

+ _Ψ₀_: an input consumer
+ _Ψ₁_: an output generator
+ _Ψ₂_: a two-port _ξi → ξo_ transformer, like a bash pipeline filter
+ _Ψ₄_: a four-port _ξi → ξo, ξo ← ξi_ transformer (full-duplex filter)

Masked aux connections include:

+ `monitor`: an output ξ used for debug/monitor info (like UNIX `stderr`)
+ `control`: an input ξ used for async control commands (like UNIX signals)

Dynamic topologies like multiplexers don't involve Ψs; they're handled directly by Γ.

Processors are keyed with unique, descriptive [η](eta.md) values that can be used to retrieve the monitor/control channels from Ξs.
