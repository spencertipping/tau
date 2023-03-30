# ψ λ scope
ψ is the unit of process allocation within τ. More specifically, a ψ is referred to by any [ξs](xi.md) that it writes to; this jointly means that:

1. ψ GC follows dataflow dependency
2. ψs' _only_ form of communication is through ξs
3. All λs within a ψ are simultaneously terminated when the ψ is destroyed

There are two types of ψs (implemented with C++ derived classes):

+ Basic ψs, which have no external interface
+ Server ψs, which register [τ](tau.md)-level ports


## GC
GC is simple: a ψ is destroyed when:

1. Nobody reads from its output ξs (or they are closed), and
2. It is not τ-pinned

τ pins arise from server ports or from an explicit pin request. A ψ can request a pin to indicate that it creates process-external side effects, e.g. writing to a file.


## Server ports
Server ports are named at the τ level:

```cpp
bool τe::bind   (St port, Sp<ψ>);            // route port to ψ
bool τe::unbind (St port, Wp<ψ> = nullptr);  // undo binding
Ξ    ψ ::connect(St port, Ξ);
```

Note that any ψ with a bound server port will be GC-pinned.

Connections are perfect forwarders of Ξ, meaning that the server can decide how to handle half/full duplexes as well as any named sidecars. The server can also transform the Ξ and returns its continuation. In most cases, this continuation is empty (that is, the server terminates the Ξ).
