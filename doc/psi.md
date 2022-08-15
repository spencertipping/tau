# Ψ
ζ-local IO is managed by Ψ, which tracks all internal connections and keeps λ states synchronized with ζ flow blocking.


## The server pattern
Open-ended components have the TCP server problem: we initiate a connection along a stable port, but must quickly change to a dedicated, dynamically-allocated port. In τ-world, just as in TCP-world, this should be done atomically -- that is, the handoff should happen with minimal context switching so we don't tie up the entry port for longer than is necessary.

We can do this with `ψm`, which allows a γ to move a port from one `ψi` to another. So far so good, but in order to close the loop we also need to be able to `ψrw()` (or `ψφcw()` maybe) on a port that hasn't yet been connected.


## Mono pipe connections
+ `ψc` has been called (i.e. ports exist)
+ `φc` has been called (i.e. ports are connected)
+ `ψx` called on one port (half-open)
+ `ψx` called on the other port, resulting in `φx` (full-closed)


## Socket connections
+ `ψc` called on server
+ `ψc` called on client
+ `φc` called to connect client to server
  + `ψm` called to move server `ψ` to persistent state
  + `ψc` called to create replacement server connection
+ `ψx` called by client or server
+ `ψx` called by the other, resulting in `φx` on client/server pipe
+ ...
+ `ψx` on the server listener

**Problem:** `φc` claims the server port, making it an invalid target for further connections. This causes a race condition wherein the second client will fail assertions as it tries to `φc` a claimed port.

**Solution:** `φcw` to wait until `φc` is ready to be called, then have `ψφw` to wait until a port is connected (for other λs)

We should also have a simple mutex/semaphore construct; a lot of this stuff boils down to classic concurrency.


## Internal state space
Now that we have the constraints above, a new set of port states falls out:

1. `!ψe`
2. `ψe && !φe`
3. `φe`

**TODO**
