# τ compiler structures
## Runtime layer
```cpp
struct ξ;              // a value pipe
struct ξi { Sp<ξ>; };  // ξ reader
struct ξo { Wp<ξ>; };  // ξ writer
struct ψ  { S<λi>; };  // multi-λ process virtual base
```

When ψ depends on inputs, it holds a reference to those `ξi` objects; each of these holds a strong `shared_ptr` to its source ψ. Closing or deleting the `ξi` causes the `shared_ptr` to decrement, eventually deleting the ψ, which can transitively delete other `ξi` and ψs.


## Compiler layer
The compiler is made of associative γ objects that construct ψs and return Ξs, which hold a hierarchically-named set of ξs.

Here are some approximate definitions; in practice each is a `struct`:

```cpp
typedef M<St, Sp<ξ>> Ξ;
typedef F<Ξ(Ξ&)>     γ;
typedef V<γ>         Γ;

Ξ Γ::operator()(Ξ&);  // apply the whole Γ
```

This provides enough machinery to do the critical steps:

1. Track ξs by name (Ξ)
2. Create ψs as necessary to connect ξs together (γ)
3. Associatively combine γs before they are applied (Γ)

Ξ provides storage for weak named ξs.
