# τ compiler structures
## Runtime layer
```cpp
struct ξ;              // a value pipe
struct ξi { Sp<ξ>; };  // ξ reader
struct ξo { Wp<ξ>; };  // ξ writer
struct ψ  { S<λi>; };  // multi-λ process virtual base

void ξi::close();      // frees ξ object
void ξo::close();      // indicates EOF
```

When ψ depends on inputs, it holds a reference to those `ξi` objects; each of these holds a strong `shared_ptr` to its source ψ. Closing or deleting the `ξi` causes the `shared_ptr` to decrement, eventually deleting the ψ, which can transitively delete other `ξi` and ψs.


## Compiler algebra
[Γ](Gamma.md) represents a _Ξ → Ξ_ transformation, **not necessarily closed under the polymorphic variant of Ξ**. For example, we might define a multiplexing Γ that accepts a duplex Ξ and returns a connection emitter, which is a lazy Ξ that collects Γs up front and applies them to each new connection later on.

Similarly, [Ξ](Xi.md) represents a smart cable of disconnected [ξ](xi.md) objects (which will presumably be connected by whichever Γ is applied next). See [Ξ spec](Xi.md) for an overview of variants.
