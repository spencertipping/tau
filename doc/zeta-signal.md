# ζ signals
[ζ](zeta.md) normally carries data, but it can also carry out-of-band signals that describe the flow of data. If sent bare, these are considered to be stream signals rather than literal data elements. (They may be embedded into other data structures, however, at which point they will be handled literally.)

[ζ loops](zeta-loops.md) describes how these signals are handled by recipients.

+ _α_: ζ has been connected to a writer (used by multiplexers)
+ _τ_: ordering within the ζ has been reset
+ _ω_: ζ has been disconnected; nothing else can be read (used by multiplexers)
