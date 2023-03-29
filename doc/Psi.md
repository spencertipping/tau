# _Ψ ∷ Ξ → ([ψ], Ξ)_
Ψs are compile-time, fixed-topology processors that can be wrapped into [Γs](Gamma.md) with adapters. There are several variants, each with optional aux connectors:

+ _Ψ₀_: an input consumer
+ _Ψ₁_: an output generator
+ _Ψ₂_: a two-port _ξi → ξo_ transformer, like a bash pipeline filter
+ _Ψ₄_: a four-port _ξi → ξo, ξo ← ξi_ transformer (full-duplex filter)

Aux connections include:

+ `monitor`: an output ξ used for debug/monitor info (like UNIX `stderr`)
+ `control`: an input ξ used for async control commands (like UNIX signals)

Dynamic topologies like multiplexers don't involve Ψs; they're handled directly by Γ.


## Ψ servers
Servers follow the same topologies as other processors, but they can define one or more named server ports. These are set up inside the main constructor:

```cpp
ΓΨ0([](ψ &q, ξi i, Ψaux)
{
  // define shared state
  q.b("foo", [&](ψ&&, Ξ x) { ... });
  for (let x : i) ...;
});
```


## Ξ conventions for aux connections
**TODO**
