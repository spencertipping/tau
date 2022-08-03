# Pipe semantics
A pipe (ψ) carries data bidirectionally, each side a serial queue (ζ) with a bounded capacity (γ) for in-flight messages. ζs are reliable and fully ordered per direction. γ = 0 when the pipe is closed; that means no further data can be sent. As a direct consequence, **pipes are never cut-through,** and can never guarantee that a message was successfully received.

Back-traffic is typically used to send control messages; e.g. κ = "writes are blocked", ι = "writes are unblocked", ω = "end of transmission".


## Control message priority
**Problem:** we want back-flow κ and ι messages to skip over buffered messages, potentially.


## Ψ/Φ
Φ boundaries interact with Ψ by receiving the read-end of one ζ and the write-end of the other ζ for a given ψ. (The other sides connect to a different Φ.)
