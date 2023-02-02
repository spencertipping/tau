# Ξ: 〈ξ〉
Mathematically, Ξ is a vector of [ξs](xi.md) used as the input and output of [γ](gamma.md) matrices.

Ξ ξs can point in either direction. This is important because some γs expect a pair of channels, one pointing each way, to perform some type of negotiation -- for example OT, throughput-dependent compression, or ED25519.

**For efficiency and semantic reasons, Ξ objects are mutable.** It isn't meaningful to fork a Ξ/γ pipeline, so γs modify the Ξ in place.


## Half and full duplex
ξs are always carried in duplexed pairs, although some connections use only one half of the pair. The connections are called `f` (for the default forward, left-to-right data movement) and `b` for backwards.


## Cabling
A Ξ contains several elements:

1. The primary data pair
2. A series of anonymous pairs, e.g. to collect circuit breakers
3. A series of named pairs, e.g. to collect specific multiplexers
4. A series of named Ξs


## γ references
Each γ takes a Ξ and returns a Ξ. In the most trivial case, we have a simple ξ → ξ transformation along the forward direction of the primary duplex channel:

```
Ξ[primary] → γf → Ξ[primary]
Ξ[primary] ←------ Ξ[primary]
```

We do this by selecting the primary duplex and splicing the forward connection:

```cpp
auto [l, r] = c.xf();  // xf() == splice forward
```

Backward splices work exactly the same way.


## ψ GC
Any leftward-pointing ξ holds a weak reference to its ψ producer. This includes bent ξs that enter from the left side but ultimately move leftwards; for example, we might have a situation like this:

```
+------------------------+
|                        |
+--> γ₁ --> γ₂ --> γ₃ ---+
        ξa     ξb     ξc
```

Here, ξc ultimately points leftwards despite having a normal data direction.

We use a simple heuristic to detect these cases: **any ξ whose output is bound before its input will hold a weak reference to that input.**
