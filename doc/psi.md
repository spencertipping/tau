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


## Internal state space
Now that we have the constraints above, a new set of port states falls out:

1. `!ψe`
2. `ψe && !φe`
3. `φe` (⇒ `ψr` and `ψw`)

(3) can proceed to either (1) or (2) depending on whether `φx` or `ψw(ω)` was called.

All of the wait-states have to do with _φ:_ we wait on 2→3 (`ψφw`) and 3→2 (`φxw`).


### `ψm` invariants
`ψm` must not be visible to waiting `ψφw` and `φxw`; if it is, then those calls will observe the ψ in an intermediate state -- although maybe the real problem is that `ψm` should create a _new_ ψ in place of the old one.

**Invariant:** `ψm` should not present a disruption in the original ψ; therefore, it should atomically replace the original.


### Denial of service
Suppose we want to rate-limit `ψm` inbound server connections. If `ψm` closes the original ψ, then we can simply not reopen it and the ψ will become unavailable.

If `ψm` does leave the original open, though, then we have to explicitly close it -- which I think is fine. We can create a higher-priority λ to `ψx` and it will take precedence, or we can maintain a state variable in the γ that governs whether the `ψm` λ should run at all. Either way, `ψm`'s ψ replacement doesn't force us to accept very inbound connection.
