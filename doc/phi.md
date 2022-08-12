# Φ
Φ boundary IO is the way τ communicates with the outside world, e.g. with files and sockets.

In both emscripten and Linux, Φ negotiates between ζs and a boundary loop that presents multiplexed, asynchronous IO events. IO contents differ between the implementations, but both use `BYTES` and UTF9 transit framing to unify the APIs. (I think it's fine to have a side-channel for structured browser stuff.)


## Connection topology
The boundary Φ is a single γ participant in the larger Γ; it has many ζs, created on-demand as fds are opened. So we have a control channel to create/destroy ζs for specific resources.
