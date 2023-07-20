# _Ψ ∷ Ξ → Ξ_
Ψs are compile-time, fixed-topology processors that can be wrapped into [Γs](Gamma.md) with adapters. There are several variants, each with optional aux connectors:

+ _Ψ₀_: an input consumer
+ _Ψ₁_: an output generator
+ _Ψ₂_: a two-port _ξi → ξo_ transformer, like a bash pipeline filter
+ _Ψ₄_: a four-port _ξi → ξo, ξo ← ξi_ transformer (full-duplex filter)

Aux connections include:

+ `monitor`: an output ξ used for debug/monitor info (like UNIX `stderr`)
+ `control`: an input ξ used for async control commands (like UNIX signals)

Either of these aux connections may be disconnected, so Ψ components must function whether or not they are present.

Dynamic topologies like multiplexers don't involve Ψs; they're handled directly by Γ.


## Defining aux connections
**TODO:** figure out what is appropriate here; maybe Ξ should have dedicated registers for these


## Ψ servers
Servers follow the same topologies as other processors, but they can define one or more named server ports. These are set up inside the main constructor, which is evaluated inline instead of in a λ. We indicate that `ΓΨ0` should do this by passing `true` as the final argument.

```cpp
ΓΨ0([](ψ &q, ξi i, Ψaux)
{
  // bind a port to a callback
  q.b("foo", [=](ψ&&, Ξ x) -> Ξ { ... });
}, ..., true);
```

You can connect to a server by calling `Ξ::c("port")`, which returns the server's result. Note that **servers don't necessarily terminate their connections;** they can act as dynamic filters or other topologies.

Server ports are τ-unique, but you can ask τ for a gensym to get new names.
