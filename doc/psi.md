# ψ λ scope
ψ is the unit of process allocation within τ. More specifically, a ψ is referred to by any [ξs](xi.md) that it writes to; this jointly means that:

1. ψ GC follows dataflow dependency
2. ψs' _only_ form of communication is through ξs
3. All λs within a ψ are simultaneously deallocated when the ψ is destroyed

There are two types of ψs (implemented with C++ derived classes):

+ Basic ψs, which have no external interface
+ Server ψs, which register [τ](tau.md)-level ports


## Server ports
Server ports are named and each inbound [Ξ](Xi.md) can be annotated with an [η](eta.md) tag. This means there are a total of three arguments to a server-port connection:

```cpp
bool τe::bind(St port, Wp<ψ>);             // route port to ψ
bool τe::unbind(St port Wp<ψ> = nullptr);  // undo binding
bool ψ::connect(St port, η tag, Ξ);        // false == reject connection
```

So, for instance, you can have a dynamic demultiplexer that creates Γs on the fly, each of which terminates to the same endpoint:

```
:%db ... sqlite_connection  # :% binds a τ port and unions outputs
(.session\k ... &db=k       # &db=k connects to τ port "db" with k tag
```

**TODO:** specify half/full duplex `&` variants
