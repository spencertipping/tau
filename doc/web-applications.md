# Web applications in τ
τ represents a steady-state factoring of your application, so the first thing to do is to define the obvious static components. The server probably has an HTTP endpoint, user-session thing, and database connection; the client has the DOM, event stream, and server connection. So we have this general architecture:

```
+-client------------------+
|           ___           |
| DOM ←--- |   |          |
|          | ψ | ↔ server |
| events → |___|          |
|                         |
+-------------------------+

+-server----------------------------------+
|                                         |
|                   | ↔ ψ ↔ |             |
| HTTP ↔ user demux | ↔ ψ ↔ | DB mux ↔ DB |
|                   | ↔ ψ ↔ |             |
|                                         |
+-----------------------------------------+
```


## Levels of synchronization
[η](eta.md) structures are portable between browser and server, so any components that use them can in principle be distributed and synchronized across the RPC gap. This creates broad flexibility in how an application's state is managed; we can have a traditional model where the client's state is periodically synchronized to the server in a series of REST-style actions, or we can keep the client and server synchronized in realtime by using OT.

The server logic is largely the same regardless of how synchronization is implemented. Here's the topology for OT-sync:

```
HTTP ↔ demux ↔ OT[client state] ↔ OT[canonical state] ↔ DB
```

**TODO:** the `demux` above needs to be a cross-mix, and another before `OT[canonical state]`. The real TODO is to specify where we have server ψs.

And here's the REST model, which can use either direct changes (the normal server pattern) or OT for extra resilience:

```
HTTP ↔ demux ↔ Δ [canonical state] ↔ DB
HTTP ↔ demux ↔ OT[canonical state] ↔ DB
```


## REST server structure
Each inbound request will be annotated by the HTTP server component with its path and headers. The auth Γ will additionally tag it with user and session information. So we'll end up with an η that looks something like this:

```
"GET" "/api/foo" u:"foo" s:"aGjDuqz..." h:(...)
```
